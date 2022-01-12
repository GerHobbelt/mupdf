# What are the Visual Studio `.sln` solution files used for?

* `mupdf` = production work
* `mupdf-dev` = bleeding edge & experimental work, that's being integrated into the toolset
* `m-dev-list` = the total list of projects, merely here to ensure we have easy access to all projects, even the **very experimental ones**
* `m-single` = the solution where you start your experimental work, porting & testing, before a project is ready to be integrated (into the `mupdf-dev` solution)


## Notes

While each of these Visual Studio Solutions has its use, the `m-dev-list` one is *quite* unwieldy and not meant for general use: its purpose is an all-around project loader and is only truly useful when:

- you are developing / debugging the `*-vcxproj.js` project patch scripts: as this solution is meant to load every project available, it should successfully load them all after we've applied our (scripted) patches.
- you are adding new `*.vcxproj` Visual Studio projects and want to make sure those load fine in Visual Studio alongside the rest. 

  > I use this solution to quickly check any new / patched project by having Visual Studio load it.

- overview of & hacking in Visual Studio files:

  I *also* use this one to copy&paste project lines into `m-single` (and the others) as I promote projects from 'scratch/experimental' to 'more-or-less-useful': I find this is faster than clicking in the IDE to add another project: by doing this bit in a generic text editor and have Visual Studio **RELOAD** the edited solution, all I have to do then is place the project(s) in the desired folder structure in the Solution.
 
  > Of course, that's my workflow. Yours may be quite different and hacking Visual Studio project and solution files may not be something you've been doing for years now and grown quite accustomed to.
  >  
  >  Therefore: YMMV (*Your Mileage May Vary*)
