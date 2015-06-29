## News ##
2014-06-24: STOB version 2.0.0 released. Added: Node cloning, deep comparison, memory leaks fixed, bugs fixed.

See [history WiKi page](ProjectHistory.md) for more complete history.


---

## Description ##
STOB is a very simple markup language, see home page for more details: [stobml.org](http://stobml.org). Example:
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


---

## Dependencies ##
This STOB C++ parser library depends on [ting](http://ting.googlecode.com) library.


---

## Download ##
There is no downloads section on Google Code anymore, so, no downloads.
Please export source code directly from SVN, for example version 2.0.0:
```
svn export http://stob.googlecode.com/svn/tags/v2.0.0 stob
```


---

## Questions and feedback ##
Please feel free to post your questions or any feedback!

Use [this](http://groups.google.com/group/libstob) mailing list.
