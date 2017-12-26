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
  - As i try to follow the rule of zero, i don't intent to implement the classes with the pimpl idiom from the start. The technical reason behind it is the pimpl object itself. The correct way of having that would be via a std::unique_ptr, but that breaks move semantics because a destructor has to be implemented (even an empty one, but it has to be there). Also, copying should be allowed which is again not possible with std::unique_ptr. This could be solved by using a std::shared_ptr and "telling" the compiler to generate the copy and move functions, but that "telling" part is what i'm bothered with. I might instead go with [the rule of zero as five defaults](http://blog.florianwolters.de/educational/2015/01/31/The_Rule_of_Zero/) which would make the intention (very) explicit and also solve the issues.
- Use of std algorithms
 
But also still the initial ideas of making it as performant as i can possible make it without making the code unbearable to read.

## A challenge in sorting!
Last time i had spend a whole lot time on getting sorting working performant. I had it very performant, but i still think it can be improved a lot. Sorting for QString object now is basically done by making a 'QCollatorSortKey' and using that to sort on. However, that also means you have all sort data at least twice in memory. One for the string you see on the screen, one for the key that the CPU finds nice to sort on. If i could change that to make a 'QCollatorSortKey' variant that can be used for both cases then some more advantages pop up (i'm not sure if it's possible at all!). Advantages are:
- No 'overhead' for creating the sort key as the visible string and sort key are one and the same.
- Much improved sorting performance on large folders, again because there is only one string.
- It prevents extra bookkeeping (sort key -> visible string mapping) thus another place where memory is saved and performance is gained.
A potential way to implement this with QCollatorSortKey is by making a own QStringView subclass that can be used to return "string data" from the QCollatorSortKey, that could work.
 
It remains to be seen if i can do this as you can't just print the sort key as string.

## QML Plugin
The earlier attempt had a QML plugin, this one shall have one as well. It might also be the revamp of Accretion, my other playground project that aimed to demonstrate this very project.
