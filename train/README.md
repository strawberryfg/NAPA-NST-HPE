# Train Code

- **End-to-end Training**
  There is only 1 model trained. The style transfer and pose regression are jointly trained.

## Train
  
  **1. Load Pretrained Model** (*Search ```load_model()``` function*)
  
  The weights are shared [here](https://drive.google.com/drive/folders/1S2Iz6N4irphM_pbYcz7YR52MtUbB6pa3?usp=sharing). By default, for the NST image transform net *F*, ```124_4```  is used. For the integral pose regression net *G*, ```118_15``` is adopted to get 2D. The depth regression net *G'* uses ```125_4``` to get depth from 2D-induced bone map. The self supervision net *F'* loads from ```126_1```. 
  
  You can easily change to any one in the model zoos by modifying the first few lines
  
  ```
  integral_model_str = '118_15'
  deconv_model_str = '124_4' 
  model_str = "125_4"
  self_sup_model_str = '126_1'
  ```
  
  **2. Start running** ```python train.py```.
  
  ----
  
  The paths where the weights are saved for net *G*, *G'*, *F*, *F'*:
  
  ```
  if ((cnt - 1) * batch_size) % per_iter_save_num == 0:
     %%%G
	 model_file = 'resnet50/model256_mpii_ist_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
     torch.save(model.state_dict(), model_file)
     
	 %%%G'
	 model_2dske_file = 'resnet50/model256_mpii_ist_2dske_to_pose_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
     torch.save(model_2dske.state_dict(), model_2dske_file)
            
     %%%F
	 model_deconv_file = 'resnet50/model256_mpii_ist_deconv_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
     torch.save(model_deconv.state_dict(), model_deconv_file)
     
	 %%%F'
	 model_self_sup_deconv_file = 'resnet50/model256_mpii_ist_self_sup_deconv_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
     torch.save(model_self_sup_deconv.state_dict(), model_self_sup_deconv_file)
                
  ```
  ----
  
  
  
  
  

- **Train 1 Pose Model for 1 Particular Style**
See [this page](https://github.com/strawberryfg/NAPA-NST-HPE/tree/main/train/per-style-training).



  