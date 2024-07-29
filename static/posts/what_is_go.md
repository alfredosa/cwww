# Go Language Learning Series

Welcome to the Go Language Learning Series! In this series of articles, we will embark on a journey to explore the Go programming language comprehensively. As I dive into the topic deeper, I want to share my learnings with other people in a concise manner. 

Let's delve deeper into Go Language. Our first stop, is understand the difference between Interpreted and Compiled Languages.

 ![Go](golang.png)

## Compiled Code vs Interpreted Code

- **Compiled:** Compiled languages translate code once before executing it. This translation results in machine code that can be directly executed by the computer's CPU. Compiled languages, like C or C++, generate machine code that can be directly executed by the computer's CPU. This compilation step usually results in faster execution.
- **Interpreted:** Interpreted languages translate instructions line-by-line while the code is being executed. Instead of generating machine code, they rely on an interpreter to execute the code. Interpreted languages, such as Python or JavaScript, do not produce machine code. Instead, they rely on an interpreter to execute the code line by line, which can make them more flexible but may lead to slower execution.

Compiled code is typically faster as it doesn't require the overhead of interpreting instructions at runtime. On the other hand, interpreted languages offer advantages like the ability to infer variable types and manage memory automatically.

Go, occupies a unique position in this spectrum. It is a compiled language, like C or C++, which means it translates code into machine code before execution. This compilation process makes Go programs efficient in terms of execution speed, similar to other compiled languages. However, Go also offers some characteristics commonly associated with interpreted languages, such as the ability to infer variable types and automatic memory management, making it a versatile and developer-friendly choice.


## Go Language Features:

Go, also known as Golang, has unique characteristics and features that set it apart:

- **Weakly object-oriented:** Go takes a different approach to object-oriented programming. It does not use traditional classes; instead, it employs structs. A struct in Go can have associated methods (functions), allowing you to encapsulate behavior with data.
- **Garbage Collection integrated:** Go includes a garbage collector, which automatically manages memory allocation and deallocation. This eliminates the need for manual memory management, reducing the risk of memory leaks.
- **Concurrency**: Go is designed to handle concurrent programming effectively. It supports running multiple tasks concurrently, and its concurrency model includes:

    - *Management* of Task Execution: Go provides tools to start and manage concurrent tasks efficiently.

    - *Communication Between Tasks*: Go uses channels to enable communication between concurrently executing tasks. Channels are a fundamental feature for safely passing data between goroutines (concurrent functions).

    - *Synchronization Between Tasks*: The select statement in Go enables task synchronization, allowing you to coordinate actions across goroutines.

    - *Goroutines*: Goroutines are lightweight threads of execution in Go. They are easy to create and are the building blocks for concurrent programming.
	
### Workspace:

In Go, you work within a structured workspace, which is organized hierarchically. The workspace typically consists of three main directories:

- Hierarchy of directories
- Common organization for sharing.

There are usually 3 different subdirectories 

- `src` - This directory contains your Go source code files.
- `pkg` - It stores packages (libraries) that you may use in your Go programs. These packages can be compiled and reused across different projects.
- `bin`: Executable files, including your compiled Go programs, are placed in this directory.
### Packages:

Go relies heavily on packages, which are used to organize and share code. Every Go program must have a package called main if it is meant to be an executable program. For example:

```go
package main
import "fmt"

func main() {
    fmt.Printf("Hello World\n")
}
```

The first line of code `(package main)` specifies the package name, and the import statement is used to include external packages. You can import packages from the standard library or third-party packages in a similar manner:

```go 
package some_package
...
...
...
```

and later can be imported as

```go
import (
	"some_package"
)

```

This structure allows you to create modular and maintainable Go programs by breaking down your code into reusable and well-organized packages.

This concludes Part 1 of our Go Learning series. In the upcoming parts, we will explore Go's syntax, data structures, advanced features, and practical applications. Stay tuned for more in-depth insights into the world of Go programming.
