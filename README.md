# Realmfall-devtools
Developer Toolkit for Realmfall and the Realmfall Engine

**IMPORTANT: WINDOWS IS NOT SUPPORTED BY THE DEVELOPER TOOLKIT**

# Compiling

We recommend using the `gcc` program on your terminal and compile the programs that you need. 

For example, to compile the Ranked Character Builder Tool, you would run the following:
```
gcc ranked_builder.c -o rankbuilder
```

# Runninng The Tools

After compiling, run the program through the terminal:
```
./rankbuilder
```


# FAQ

### Why Make The Devtools Public?
The Realmfall repository itself is private, mainly to preserve server integrity and avoid leaks of upcoming content. Think of the devtools as a spoiler-free API that you can use to create datapacks for Realmfall; the devtools are intended for both internal developers and the community to use, so that everyone may have a chance to contribute their own content to the Realmfall experience.

### Why Linux-only?
The Devtools are written in C, which is far more performant and does not require the full tech stack of Realmfall to run. It's a difference between only a few megabytes to over 4 GB needed to "get things done." The main developers in Team Biscuit either use Arch Linux, WSL, or both. There is no official support for the devkit on the Windows platform, but you can try! We cannot guarantee it will work. Due to the manner of C being low-level, it is not like Java where any OS can interpret it.

### Why C?
Easier to write, runs faster than Java, and the mental illnesses of the Salt of Hope is a case study to look into.

# Licenses

The Realmfall Devkit is licensed under the GNU General Public License.

cJSON is bundled with its custom license. Source: [GitHub - cJSON](https://github.com/DaveGamble/cJSON)