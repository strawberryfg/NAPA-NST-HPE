import argparse
import time
import numpy as np

import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim

import torchvision.transforms as transforms
import torchvision.models as models
import torchvision.datasets as datasets
from torch.utils.data import DataLoader, Dataset

from models import *
from utils import MyDataset, MPIIDataset, load_image, save_image, gram_matrix, normalize_batch
from mpii import MPII
import os

def main():
    parser = argparse.ArgumentParser(description='Style Transfer')
    parser.add_argument('--batch-size', type=int, default=4, action='store')
    parser.add_argument('--epochs', type=int, default=2, action='store')
    parser.add_argument('--content-weight', action='store', type=float, default=1e5)
    parser.add_argument('--si', action='store', type=str, default='la_muse.jpg')
    parser.add_argument('--style-weight', action='store', type=float, default=1e10)
    parser.add_argument('--style-path', action='store', type=str, default='/allstyles')
    parser.add_argument('--cuda', action='store_true', default=False)
    args = parser.parse_args()

    device = torch.device("cuda" if torch.cuda.is_available() and args.cuda else "cpu")

    batch_size = args.batch_size
    learning_rate = 1e-3
    epochs = args.epochs
    content_weight = args.content_weight
    style_weight = args.style_weight
    log_interval = 100

    transformer = ImageTransformationNetwork().to(device)   
        
    optimizer = optim.Adam(transformer.parameters(), learning_rate)
    mse_loss = torch.nn.MSELoss()

    transform = transforms.Compose([transforms.Resize(256),
                                    transforms.CenterCrop(256),
                                    transforms.ToTensor(),
                                    transforms.Lambda(lambda x: x.mul(255))])
    #train_dataset = MyDataset('/scratch/ozl201/Vision/images', transform)
    
    d1 = MPII('train')
    d2 = MPII('test')
    d1 = [d for d in d1.load_data()]
    d2 = [d for d in d2.load_data()]
    dataset = d1 + d2
    train_dataset = MPIIDataset(dataset)
    train_loader = DataLoader(train_dataset, batch_size=batch_size, num_workers = 1)

    loss_network = LossNetwork(requires_grad=False).to(device)

    style_image = args.si
    print(style_image)
    style_path = args.style_path
    style = load_image(os.path.join(style_path, style_image))
    style_transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Lambda(lambda x: x.mul(255))
    ])
    style = style_transform(style)
    style = style.repeat(batch_size, 1, 1, 1).to(device)

    features_style = loss_network(normalize_batch(style))
    gram_style = [gram_matrix(y) for y in features_style]

    for e in range(epochs):
        transformer.train()
        agg_content_loss = 0.
        agg_style_loss = 0.
        count = 0
        for batch_id, (x, _) in enumerate(train_loader):
            n_batch = len(x)
            count += n_batch
            optimizer.zero_grad()

            x = x.to(device)
            y = transformer(x)

            x = normalize_batch(x)
            y = normalize_batch(y)

            features_y = loss_network(y)
            features_x = loss_network(x)

            content_loss = content_weight * mse_loss(features_y.relu2_2, features_x.relu2_2)
            
            style_loss = 0.
            for ft_y, gm_s in zip(features_y, gram_style):
                gm_y = gram_matrix(ft_y)
                style_loss += mse_loss(gm_y, gm_s[:n_batch, :, :])
            style_loss *= style_weight
            total_loss = content_loss + style_loss
            total_loss.backward()
            optimizer.step()

            agg_content_loss += content_loss.item()
            agg_style_loss += style_loss.item()
            if (batch_id + 1) % log_interval == 0:
                msg = "{}\tEpoch {}:\t[{}/{}]\tcontent: {:.6f}\tstyle: {:.6f}\ttotal: {:.6f}".format(
                    time.ctime(), e + 1, count, len(train_dataset),
                                  agg_content_loss / (batch_id + 1),
                                  agg_style_loss / (batch_id + 1),
                                  (agg_content_loss + agg_style_loss) / (batch_id + 1)
                )
                print(msg)
            if count == len(train_dataset):
                transformer.eval().cpu()
                save_model_filename = style_image.replace('.', '_') + "_epoch_" + str(e+1) + "_CW_" + "{:.0e}".format(content_weight) + "_SW_" + "{:.0e}".format(style_weight) + ".model"
                save_model_path = os.path.join(os.path.abspath('style_weights'), save_model_filename)
                torch.save(transformer.state_dict(), save_model_path)
                transformer.to(device).train()
    transformer.eval().cpu()
    save_model_filename = style_image.replace('.', '_') + "_epoch_" + str(epochs) + "_CW_" + "{:.0e}".format(content_weight) + "_SW_" + "{:.0e}".format(style_weight) + ".model"
    
    if not os.path.isdir(os.path.abspath('style_weights')):
        os.mkdir(os.path.abspath('style_weights'))
    
    save_model_path = os.path.join(os.path.abspath('style_weights'), save_model_filename)
    torch.save(transformer.state_dict(), save_model_path)

    print("\nDone, trained model saved at", save_model_filename)
        
if __name__ == "__main__":
    main()