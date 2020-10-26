**puu** is a minimalistic general purpose markup language, proposed by Ivan Gagis in July 2012.
The name **puu** just means _tree_ in Finnish. **puu** allows describing of string hierarchies.

# Syntax
puu syntax is very simple and intentionally kept minimalistic.
Here are the syntax rules:

1. The only type of language element is a string of text.
  - String is generally enclosed in double quotes. Double quotes can be omitted if string does
    not contain any of the following special characters: space, tabulation, line feed,
    carriage return, left and right curly brackets, double quote.
  - If the string is enclosed in double quotes then tabulations, line feeds and carriage returns
    inside of that string should be ignored by the parser.
  - If the string is enclosed in double quotes then the following escape sequences inside that
    string should be recognized:
    + **\n** - line feed
    + **\r** - carriage return
    + **\"** - double quote
    + **\\\\** - back slash
    + **\t** - tabulation

1. String can have arbitrary number of child strings. Those are optionally listed in the curly
  brackets following the string. If string does not have any children then the curly brackets can be omitted.
1. Parts of a document can be commented. C++ commenting rules apply.
  - A line of the document can be commented using double slash **//** sequence. Everything past the double
    slash and till the end of the line will be ignored by the parser.
  - A multi-line comment is achieved using C-style commenting, i.e. everything between **/*** and **\*/** is ignored
    by the parser. Nested comments are not supported.
  - Comments are not allowed inside strings enclosed in double quotes.

# Example
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

        /* multi-line
           comment */

        "multi-line
         string"

        "Escape sequences \" \n \r \t \\"
    }
}
```

# Libraries
- [C++](https://github.com/igagis/puu)
- [C#](https://github.com/igagis/puu-cs)
- [Java](https://github.com/igagis/puu-java)

# Contact
[Site admin](mailto:igagis@gmail.com)
