GeneticImageEvolution

The program evolves images out of primitives like ellipses with a genetic algorithm. The GUI is designed with Qt, you will need that to run the code
(also you will need the Qt DLLs to run the windows build, but build it yourself to be sure).

This program was written in German, so there will still be German comments, variable names etc. However I tried to translate the GUI to english so it may be understood by non Germans.

With the program, you can select an image or an image sequence (all images should be the same size) and run the genetic algorithm with either ellipses, triangles or circles. The number of primitives and individuals can be varied. The less of both you select, the faster it will run, however the results will not be as good.

For the best results, I recommend using smaller black and white images (or selecting the "black and white" checkbox in settings) with at least 1000 primitives and 100 individuals. If you want to make a video out of the process, select the checkbox "snapshot every x generations " in "other" in the settings. This will save the best image every x generations to the folder you selected.

Not all settings will work properly or even do anythin, as the project was never fully finished.

If there are any more questions, I am happy to help you out!

Andi