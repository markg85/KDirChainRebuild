# KDirChainRebuild V2
## Rationale
Years ago this project started as an experiment to see if the API for KDirLister, KDirModel and KFileItem could be cleaned up. 
It seemed to be heading in the right direction, but never matured any further beyond a experiment. 
 
Fast forward about 5 years. I know a lot more about C++ now then i knew back then, also i see real issues in todays code (KDirLister, KDirModel, and KFileItem) that just cannot be solved in KIO at the moment due to API promises.
A - rather big - example there would be move semantics, those are not porrible due to verious reasons. 
 
So i want to restart this experiment to rebuild those 3 classes from the ground up (again) with modern C++ in mind and high performance. 
It might become material for a hypothetical "KDE Frameworks 6.xx", but don't bet on it. Thus far this is merely a playground for me, lets see what comes out of it.
 
## Differences compared to my earlier attempt
... which you can find in the 'ancient' branch. For historical purposes. 
The main differences is use of modern C++ features:
- Move semantics
- Rule-of-zero (let the compiler do the hard work)
- Use of std algorithms
 
But also still the initial idead of making it as performant as i can possible make it without making the code unbearable to read.

## QML Plugin
The earlier attempt had a QML plugin, this one shall have one as well.
