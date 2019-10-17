# puu [![releases](https://img.shields.io/github/tag/igagis/puu.svg)](https://github.com/igagis/puu/releases) [![Build Status](https://travis-ci.org/igagis/puu.svg?branch=master)](https://travis-ci.org/igagis/puu) [![Build status](https://ci.appveyor.com/api/projects/status/owv6srivorp08lxu/branch/master?svg=true)](https://ci.appveyor.com/project/igagis/puu/branch/master)



A lightweight puu markup language C++ library.

See [WiKi](wiki/HomePage.md) for installation instructions.

puu document example:
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

		//raw string
		R"qwerty(raw string content " """ { } \ / )qwerty"
	}
}
```
