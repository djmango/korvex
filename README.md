# korvex

### a repository of all the software written for korvex robotics

------

## what is VEX robotics?

VEX robotics is a STEM program that allows the future innovators, thinkers and problem solvers of America showcase their talents. The program’s goal is to create engaging, affordable and powerful solutions that immerse students in engineering challenges through the excitement of building and programming robots 

## writing and compiling a basic PROS program

1. All functions are thoroughly explained in the [PROS documentation](https://pros.cs.purdue.edu/v5/api/index.html). The syntax is all standard C++, so [Stackoverflow](https://stackoverflow.com/) will be an excellent resource for problem solving. All the files you want to edit are in the src folder. opcontrol is for user control functions and loop, and auto is for auton code. 
2. The following is a basic function that allows for chassis control.![img](https://lh3.googleusercontent.com/4uhuqIEbZxsJ41i5F7IFF58_EQf2IzKnMJoBWgrTMe5Sy-io0iFBCVR6hc8zMfyx-4Df1tq734W_UDF7DyzSKQNUnP7OxKWMDbRp3iRfLOzwrcQ0WC5usUnvEtQ6bl7Gzudo4LGR)

This code can be found [here](https://github.com/djmango/korvex/blob/master/InTheZone/KorvexV1/src/opcontrol.c). All documentation for this code can be found in the code itself, and is outlined [here](https://github.com/djmango/korvex/tree/master/InTheZone/README.old.md)

1. In order for the function to be run while the robot is in user control mode, it must be in the operatorControl loop. It can be located at the end of the opcontrol file, in the src folder. Here is an example of the control loop

![img](https://lh5.googleusercontent.com/JIGfrzqswmYMGX4p2XXwcGUZI0WM3RzqwOdtRw4Xoh3QVThOFzb_Smh9xJPbgbqi4ZCCPbKChZ3U3kquBZbf-Rs4xObBIAkBHWxVVGMyT4SxCjBRgH9oirlrnWgHQs0WVw7532zJ)

1. When the code is complete, open a terminal in the parent folder. Here is the what the final file tree should look like![img](https://lh5.googleusercontent.com/QEtypP8GkS0ByjxB_YIdCOr85BvLZ-dR-CFbHC3yK1Ec1-JGoaTJOkk5HtXNQPd5SXUrAN_5KZAxQBVsLCALjeFqMH3UE8OxYgpwHRIkBKsbCnOepbIPuDjb0D9MVx_Q0FeYWN9r)
2. Navigate to the parent folder in the terminal, the folder above src. On Windows, you can open the terminal in the file explorer by pressing Shift+Right Click and selecting the Open Powershell or Open Command Prompt option. Your terminal should look something like this![img](https://lh3.googleusercontent.com/ByR26qbAQnzd7-d0ka-Wa3YkeHp2nCq3fW_sM0Z3flPOOgarrsLTygdqaN8DUagL5wNwLBwLxJH_QL_Mh-Ksz6hsMtp5XcPsWsibdJpj1m4JYqbKxZaMVx66xCNLhd7xJjrQ3Dy-)
3. Type ls or dir (Windows) in the terminal. The result should look something like this![img](https://lh3.googleusercontent.com/4xAtic_Mq_38RfFKfeG2h1kbX5otFLlpZ2wiK8pj_kDaM2H2ITiIKdYEgASsJuFaNSUI4EGosUOoW39w9dBnQjzUpu1RClXE1ueOJKPirUiTNZUpFCuKpA4RKhl4LHkwGJka66b9)

This ensures you are in the correct directory

1. Connect the robot to your computer and type pros mu into the terminal.![img](https://lh3.googleusercontent.com/SRTVxNUObkQGGJ3wO72Zr3iIRPzJMNzeJehSA_osrG6PsFICP3HP48E2p8W9uxIfh8aBDz7rOzcH-3tEuyFvLNgcu7aV_XQHwdsIKv6HO-A13tEwKy7m8w3__NOGWi1cf0TYDMiu)
2. Press enter to run this command. If there is an error in your code, it will spit it out here, otherwise, your code will be flashed to the robot. When it is complete, disconnect from the robot and restart the cortex. Your code is now running on the robot.

## contributing

1. Download [GitHub Desktop](https://desktop.github.com/)
2. Sign up/Log in to your own GitHub account
3. Clone the [Korvex Repository](https://github.com/djmango/korvex.git) in GitHub Desktop![img](https://lh6.googleusercontent.com/gZbZLBOrxRRPeolDaJm2wX8DJXvf-nKCCTUI7GqF71pDOw8TAtjV_4G2XutL7tg4qTUhT7jFjhNj9Wdcr5XSjk_VEc4c6FzXgdXEmTvYPwGKlPlum5_3F-wjz2gEPRAjy4aeyVuX)
4. If you are a verified contributor to Korvex Robotics, select your branch from the top menu. If there is not a branch with your name on it, make a new one
5. Make your intended changes
6. When your changes are complete, commit your changes and add a small paragraph detailing what you modified, then push

## resources

1. C++ Basics
   1. [C++ Cheat Sheet](https://github.com/gibsjose/cpp-cheat-sheet)
   2. [C++ Crash Course](https://www.labri.fr/perso/nrougier/teaching/c++-crash-course/index.html)
   3. [C++ Basic Implementation](https://www.programiz.com/cpp-programming/return-reference)
2. Working with a CLI
   1. At least skim through [this](https://github.com/jlevy/the-art-of-command-line/blob/master/README.md)
3. Version Control (git)
   1. [Intro to GitHub](https://guides.github.com/activities/hello-world/)
   2. [Intro to git](https://guides.github.com/introduction/git-handbook/)
4. PROS
   1. [PROS first time user guide](https://pros.cs.purdue.edu/v5/getting-started/new-users.html)
   2. [PROS getting started guide](https://pros.cs.purdue.edu/v5/getting-started/index.html)

