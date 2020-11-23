import os
from PIL import Image
from torch.utils.data import DataLoader, Dataset
import cv2
import torchvision.transforms as transforms
import numpy as np

class MyDataset(Dataset):
    def __init__(self, image_path, transform=None):
        self.image_paths = [os.path.join(image_path, p) for p in os.listdir(image_path)]
        self.transform = transform

    def __getitem__(self, index):
        image_path = self.image_paths[index]
        x = Image.open(image_path)
        if self.transform is not None:
            x = self.transform(x)
        return x, image_path
    
    def __len__(self):
        return len(self.image_paths)
        
class MPIIDataset(Dataset):
    def __init__(self, db):
        self.db = db
        self.transform = transforms.Compose([
                                    transforms.ToTensor(),
                                    transforms.Lambda(lambda x: x.mul(255))])

    def __getitem__(self, index):
        db_image = self.db[index]
        image_path = db_image['img_path']
        content_image = cv2.imread(image_path, cv2.IMREAD_COLOR | cv2.IMREAD_IGNORE_ORIENTATION)
        x, trans = generate_patch_image(content_image, db_image['bbox'], False, 1.0, 0)
        
        img_channels = 3
        color_scale = [1.0, 1.0, 1.0]
        for i in range(img_channels):
            x[:, :, i] = np.clip(x[:, :, i] * color_scale[i], 0, 255)
        
        x = self.transform(x)
        return x, image_path
    
    def __len__(self):
        return len(self.db)
		
def load_image(filename, size=None, scale=None):
    img = Image.open(filename)
    if size is not None:
        img = img.resize((size, size), Image.ANTIALIAS)
    elif scale is not None:
        img = img.resize((int(img.size[0] / scale), int(img.size[1] / scale)), Image.ANTIALIAS)
    return img


def save_image(filename, data):
    img = data.clone().clamp(0, 255).numpy()
    img = img.transpose(1, 2, 0).astype("uint8")
    img = Image.fromarray(img)
    img.save(filename)


def gram_matrix(y):
    (b, ch, h, w) = y.size()
    features = y.view(b, ch, w * h)
    features_t = features.transpose(1, 2)
    gram = features.bmm(features_t) / (ch * h * w)
    return gram


def normalize_batch(batch):
    mean = batch.new_tensor([0.485, 0.456, 0.406]).view(-1, 1, 1)
    std = batch.new_tensor([0.229, 0.224, 0.225]).view(-1, 1, 1)
    batch = batch.div_(255.0)
    return (batch - mean) / std

def generate_patch_image(cvimg, bbox, do_flip, scale, rot):
    img_size = 256
    img = cvimg.copy()
    img_height, img_width, img_channels = img.shape

    bb_c_x = float(bbox[0] + 0.5*bbox[2])
    bb_c_y = float(bbox[1] + 0.5*bbox[3])
    bb_width = float(bbox[2])
    bb_height = float(bbox[3])

    if do_flip:
        img = img[:, ::-1, :]
        bb_c_x = img_width - bb_c_x - 1

    trans = gen_trans_from_patch_cv(bb_c_x, bb_c_y, bb_width, bb_height, img_size, img_size, scale, rot, inv=False)
    img_patch = cv2.warpAffine(img, trans, (img_size, img_size))
    #print(img_patch)
    
    img_patch = img_patch[:,:,::-1].copy()
    return img_patch, trans

def gen_trans_from_patch_cv(c_x, c_y, src_width, src_height, dst_width, dst_height, scale, rot, inv=False):
    # augment size with scale
    src_w = src_width * scale
    src_h = src_height * scale
    src_center = np.array([c_x, c_y], dtype=np.float32)
    # augment rotation
    rot_rad = np.pi * rot / 180
    src_downdir = rotate_2d(np.array([0, src_h * 0.5], dtype=np.float32), rot_rad)
    src_rightdir = rotate_2d(np.array([src_w * 0.5, 0], dtype=np.float32), rot_rad)

    dst_w = dst_width
    dst_h = dst_height
    dst_center = np.array([dst_w * 0.5, dst_h * 0.5], dtype=np.float32)
    dst_downdir = np.array([0, dst_h * 0.5], dtype=np.float32)
    dst_rightdir = np.array([dst_w * 0.5, 0], dtype=np.float32)

    src = np.zeros((3, 2), dtype=np.float32)
    src[0, :] = src_center
    src[1, :] = src_center + src_downdir
    src[2, :] = src_center + src_rightdir

    dst = np.zeros((3, 2), dtype=np.float32)
    dst[0, :] = dst_center
    dst[1, :] = dst_center + dst_downdir
    dst[2, :] = dst_center + dst_rightdir

    if inv:
        trans = cv2.getAffineTransform(np.float32(dst), np.float32(src))
    else:
        trans = cv2.getAffineTransform(np.float32(src), np.float32(dst))

    return trans

def rotate_2d(pt_2d, rot_rad):
    x = pt_2d[0]
    y = pt_2d[1]
    sn, cs = np.sin(rot_rad), np.cos(rot_rad)
    xx = x * cs - y * sn
    yy = x * sn + y * cs
    return np.array([xx, yy], dtype=np.float32)