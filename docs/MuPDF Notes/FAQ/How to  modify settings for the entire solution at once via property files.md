# How to modify compile/link/build settings for the entire solution at once (via `.props` property files)?

Use the Visual Studio **Property Manager** -- see how to enable that one in the Visual Studio IDE [here](https://stackoverflow.com/questions/10179201/cannot-find-property-manager-option-in-visual-studio-not-express-version/12889223#12889223) -- as it's not yet present in a default install.

In Visual Studio, then go to Views -> Other Windows... -> Property Manager.

This should give you a ''*Property Manager*" tab in the right column, where you've got your *Solution Manager*, *Team Explorer* and *Resource View* tabs already.

In the Property Manager pick any C/C++ project and twirl open the property list. You should now observe the various `*.props` property files we keep in the `platform/win32/` directory next to the solution and project files:

- `common-project.props`is intended to contain any settings that are applied to **all projects** and **all build modes**.
- `common-project-(Release|Debug)-Win(32|64).props` are the files meant to carry build mode specific settings.
- `common-project-ultimate-overrides.props` is a last-resort cop-out property file for those who need to test compiler or other settings while testing that stuff; only touch this when you want to bluntly override any and all previous configurations: this property list is loaded last and [as the Microsoft documentation states](https://docs.microsoft.com/en-us/cpp/build/reference/vcxproj-file-structure?view=msvc-160), these properties will therefor have precedence over anything else.

You can easily edit the settings in the IDE then by double-clicking on the property file of your choice.

## Notes

- **Usage perspective**: It does not matter in what build mode (Release/Debug/32/64) or project you picked those global property files listed in the section above: when, for example, you edit the `common-project` properties, they are automatically applied to **all** projects.
- **Deep behavioral hacking perspective**: This behaviour is driven by the way the `update-vcxproj.js` script modifies the Visual Studio projects. When you create C/C++ projects from scratch, it is strongly advised to run this script on the `.vcxproj` project file to ensure its build settings are compatible with the remainder of the bunch.

- **Usage perspective**: Apparently it's impossible in the IDE to edit some important build settings (Whole Program Optimization, JUMBO support, etc.). Until we've found how to set those from those same `.props` property files, the *old skool* approach remains:
	- Either select all projects [[Which are the special projects|except the special ones]] and right-click in the Solution Explorer to get at the (common denominator) Properties dialog, where you change them all.
	- Or patch the `update-vcxproj.js` script and apply globally:
	
	  ```sh
	  for f in *.vcxproj ; do
	    node ./update-vcxproj.js $f
	  done
	  ```
	  
	  Which looks like this when typed on a single line (shell, command prompt):
	  
	  ```sh
	  for f in *.vcxproj ; do node ./update-vcxproj.js $f ; done
	  ```
	  
