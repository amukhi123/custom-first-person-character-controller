# Custom First-Person Character Controller (Unreal Engine)

## Description

 A custom first-person character controller built in Unreal Engine 5.3 that features slope movement, dynamic footsteps, walking, looking, jumping, crouching, sprinting and head bobbing.

 All properties can be customized in the blueprint, "BP_FPController" located in Content/Blueprints/BP_FPController.

 All controls can be changed by going to the input mapping context, "IMC_FirstPersonCharacterController" located in Content/Input/IMC_FirstPersonCharacterController.

## Controls

WASD - Movement
<br>Mouse - Look
<br>Left Shift - Sprint
<br>Space Bar - Jump
<br>Left Control - Crouch

## Gallery

### **Click the thumbnail to play the video**

[![Grappling Hook Gun Implementation Showcase](https://img.youtube.com/vi/_tYSphYUr-s/0.jpg)](https://youtu.be/_tYSphYUr-s)

## How to Use

1. Clone repository
2. Open repository in Unreal Engine:
   1. Double-click on the FPController.uproject file OR
   2. Right-click the FPController.uproject file, select "Generate Visual Studio project files", open the .sln file and build the project
3. Navigate to Content/Blueprint/BP_FPController
4. Right-click AssetActions -> Migrate...
5. In the file explorer navigate to the Content folder of your project and click "Select Folder"

If you would like to see the custom first-person character controller in action you can download the lastest release (fp-controller-1.0.0.zip). Then run "FPController.exe".

## Tech Stack

Made using C++, Unreal Engine and git.

## Resources

[Super Grid](https://www.unrealengine.com/marketplace/en-US/product/supergrid-starter-pack)
<br>[Footstep Sounds](https://opengameart.org/content/foot-walking-step-sounds-on-stone-water-snow-wood-and-dirt)
