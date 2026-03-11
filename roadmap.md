## Roadmap for clang-bang

I plan to plan and do the following things over time, but C++ is not my full-time job **at the moment**. I created this because it was necessary at a hobby + professional level. Keep in mind that everything here is a product of my free time.  
Anyway, I will add my thoughts for the roadmap below. Some of these will be like thinking out loud.

1. [ ] Stop clang-bang from being a Windows-specific repository. For example, make the LLVM toolset usable with Ubuntu 24 or Debian 12 as well.
2. [ ] Add overlay-ports for ports that cannot be compiled with Clang [see footnote 1, see footnote 2].
3. [ ] Although I am not entirely sure of their necessity, prepare standalone, clean docker images: llvm + vcpkg. 
   I do not plan to upload the images to the hub, but the dockerfiles can be found in the repository. I have done this many times specifically for my own projects in Linux environments.

## footnotes

1. Clang's include rules and other standard implementations are in most cases much stricter than msvc and gcc; therefore, most common packages are currently not usable directly with clang. To give an example, clang is very strict about includes. For example, you want to use `std::construct_at` but do not include the header where it is defined, this code might compile in gcc or other mainstream compilers (in most cases), while clang might give errors like "I could not find it, go define it."  
The fact that other compilers compile without errors does not mean they are doing it correctly; they might have internally included that header itself, but this is not guaranteed.

2. I am not talking about all ports. Only highly used packages. To give an example: gRPC, Poco, Zlib etc. I have no intention of copying the entire vcpkg.  
To be honest, I will only do this for the ports I use and experience problems with.
