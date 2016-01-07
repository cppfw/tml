# stob [![releases](https://img.shields.io/github/tag/igagis/stob.svg)](https://github.com/igagis/stob/releases) [![Build Status](https://travis-ci.org/igagis/stob.svg?branch=master)](https://travis-ci.org/igagis/stob) [![Build status](https://ci.appveyor.com/api/projects/status/owv6srivorp08lxu/branch/master?svg=true)](https://ci.appveyor.com/project/igagis/stob/branch/master)



A lightweight STOB markup language C++ library. See more info at http://stobml.org

See [WiKi](wiki/HomePage.md) for installation instructions.

STOB document example:
```
"String object"
AnotherStringObject
"String with children"{
	"child 1"
	Child2
	"child three"{
		SubChild1
		"Subchild two"

		Property1 {Value1}
		"Property two" {"Value 2"}
		//comment

		/* multi line
		   comment */

		"Escape sequences \" \n \r \t \\ \/"
	}
}
```
