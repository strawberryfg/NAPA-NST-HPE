import argparse
import os
from collections import namedtuple
import torch
import torchvision.transforms as transforms
import time
import random

from models import *
from utils import load_image, save_image

def main():
    t0 = time.time()
    parser = argparse.ArgumentParser(description='Stylize Image')
    parser.add_argument('--w', action='store')
    parser.add_argument('--i', action='store')
    parser.add_argument('--s', action='store', default=0, type = int)
    parser.add_argument('--sw', action='store', default='5e+10')
    parser.add_argument('--cw', action='store', default='1e+05')
    parser.add_argument('--cuda', action='store_true', default=False)
    parser.add_argument('--output-folder', action='store', type = str, default='/scratch/ozl201/Vision/mpii/outputs/')
    args = parser.parse_args()
    
    device = torch.device("cuda" if torch.cuda.is_available() and args.cuda else "cpu")
    
    weights = args.w
    image = args.i
    sample = args.s
    sw = args.sw
    
    weights = weights.replace('.', '_')

    model = ImageTransformationNetwork().to(device)
    weights_filename = weights + '_epoch_2_CW_' + args.cw + '_SW_' + sw + '.model'
    
    weights_path = os.path.join(os.path.abspath('style_weights'), weights_filename)
    model.load_state_dict(torch.load(weights_path))
    
    output_folder = args.output_folder + weights + '/'
    
    if not os.path.isdir(output_folder):
        os.mkdir(output_folder)
    
    completed_images = [p for p in os.listdir(output_folder)]
    
    image_folder_path = os.path.abspath('mpii/images/')
    images = [p for p in os.listdir(image_folder_path)]
    
    if sample > 0:
        images = random.sample(images, sample)
        
    content_transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Lambda(lambda x: x.mul(255))
    ])
        
    t1 = time.time()
    print("Preprocessing time: {}".format(t1-t0))
    image_time = 0
    
    if image:
        t0 = time.time()
        content_image = load_image(os.path.join(image_folder_path, image))
        content_image = content_transform(content_image).to(device)
        content_image = content_image.unsqueeze(0)
        output = model(content_image).cpu()
        output_path = os.path.join(output_folder, image)
        save_image(output_path, output[0].detach())
        
        del output
        
        t1 = time.time()
        print("Image time: {}".format(t1-t0))
        image_time += t1-t0
    else:
        for image in images:    
            if image not in completed_images:
                t0 = time.time()
                content_image = load_image(os.path.join(image_folder_path, image))

                content_image = content_transform(content_image).to(device)
                content_image = content_image.unsqueeze(0)
                output = model(content_image).cpu()
                output_path = os.path.join(output_folder, image)
                save_image(output_path, output[0].detach())
                
                del output
                
                t1 = time.time()
                print("Image time: {}".format(t1-t0))
                image_time += t1-t0
    
    print("Average time: {}".format(image_time/len(images)))
    

if __name__ == "__main__":
    main()