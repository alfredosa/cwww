# Go Learning Series - Part 3: Advanced topics

Here are some of the learnings that took me more time to understand in the go learning journey. After doing a Discord Bot using the discordgo library and playing/building some integrations with APIs, I had to battle my way through some more advanced topics. This made me appreciate a lot more Go to be honest.

## Concurrent Programming: Defer

```go
defer Action()
```

The defer is one of the coolest things I found out about in Go. It allows you to delay the execution of an action until the very end of the function.

In Go, the `defer` keyword is used to schedule a function call to be executed later, here is a clearer example:

```go
package main

import "fmt"

func main() {
    defer fmt.Println("Deferred 1")
    defer fmt.Println("Deferred 2")
    fmt.Println("Hello,")
    fmt.Println("World!")
}
```

In this example, when you run the main function, the output will be:

```txt
Hello,
World!
Deferred 2
Deferred 1
```

That means that you can plan the end of your function within your code. Common use cases are:

- Closing files, connections, or other resources to ensure they are properly released when you're done with them.
- Unlocking mutexes or releasing locks to prevent deadlocks.
- Logging and tracing to record function entry and exit points for debugging and monitoring purposes.
- Recovering from panics by deferring a function that calls recover() to handle panics gracefully.

Obviously, with great power, comes great responsability and it should be used with care to avoid any unecessary complexity in your code. 

## Concurrent Programming: Goroutines

Goroutines are a fundamental concept in Go for concurrent programming, they allow you to perform tasks concurrently. 

You can think of goroutines as independently executing functions or methods that can run concurrently with other goroutines.

Creation: Creating a goroutine is as simple as prefixing a function or method call with the go keyword. For example:

```go
go myFunction() // Start a new goroutine to execute myFunction()
```

here's a simple example that shows how powerfull goroutines can be

```go
package main

import (
    "fmt"
    "sync"
)

func calculateSquare(number int, wg *sync.WaitGroup) {
    defer wg.Done() // Decrement the wait group counter when the goroutine exits
    result := number * number
    fmt.Printf("Square of %d is %d\n", number, result)
}

func main() {
    var wg sync.WaitGroup // Create a WaitGroup to wait for all goroutines to finish

    numbers := []int{1, 2, 3, 4, 5}

    // Launch a goroutine for each number in the list
    for _, num := range numbers {
        wg.Add(1) // Increment the wait group counter for each goroutine
        go calculateSquare(num, &wg)
    }

    // Wait for all goroutines to finish
    wg.Wait()

    fmt.Println("All calculations are done!")
}
```

Now you will see that we also included an example of `defer` which is also pretty cool. Allows you to postpone the decrement of the wait group counter only after the function finishes. But ok - enough of defer.

But goroutines allow you to perform concurrent tasks with a super simple syntax. I thought this was cool.

## The underscore assignment `_`

In Go, the assignment statement using underscores _ is used when you want to discard or ignore the values returned by a function or an expression. This is often used when you are not interested in all the values returned by a function call, but you want to capture only a subset of them or simply ignore them.

The underscore `_` is a special identifier in Go that serves as a write-only variable. It effectively discards the value assigned to it. For example, if a function `something.action()` returns multiple values, but you are interested in only the second value, you can use the underscore to discard the first value:

```go
_, secondValue := something.action()
```

## Error Handling 

Something that was new to me coming from Python or Rust, was the common practice to return 2 values in functions. Mor explicitly, usually a variable and an error. In Go, it is a common practice to use the var, err pattern for functions and return both a value and an error. This is considered a best practice in Go for several reasons:

```go
var, err := something.action()
```

- Makes error Handling easier to understand: Go encourages explicit error handling rather than relying on exceptions or panics. By returning an error along with the result, it makes it clear that a function can fail, and it forces the caller to handle potential errors explicitly. This helps in writing robust and predictable code.
- Predictability: By returning an error value, it's clear when a function can potentially return an error, and it gives the caller the opportunity to decide how to handle it.
- Testing enhanced. Error values are easier to test than panics.
- Easier to understand clearly what the function contract is. When you return an error, it becomes part of the function's contract.

Takeaway here is that using `var, err` for functions and returning errors is considered a best practice in Go because it promotes error handling, predictability, and robust code. Panics should be reserved for exceptional and unrecoverable situations.

## Typing syntax

It's not a super advanced topic, but I thought it was neat to have this syntaxing and the fact that in Go, you can create your own types. The way you type variables on receipt and return 

```go
func run(var1 string, var2 int) (string, error) {
    
    if var2 != 3 {
        return var1, fmt.Errorf("var2 is a good number")
    }
    
    return var1, nil
}
```

For custom types, you can simply do this:

```go
type Temperature float64

func main() {
    // Declare variables of the custom type
    var roomTemp Temperature = 72.5
    var boilingPoint Temperature = 100.0

    // Perform operations with custom type variables
    temperatureDifference := boilingPoint - roomTemp
    fmt.Printf("The temperature difference is %.2f degrees Celsius.\n", temperatureDifference)

    // You can also use type conversion to assign values
    temperatureInFahrenheit := Temperature(32)
    fmt.Printf("32 degrees Fahrenheit is %.2f degrees Celsius.\n", temperatureInFahrenheit)
}
```

Or something much more complex: 

```go 
// Define a custom struct type named "Person"
type Person struct {
    FirstName string
    LastName  string
    Age       int
    Email     string
}

func main() {
    // Create instances of the custom type
    person1 := Person{
        FirstName: "John",
        LastName:  "Doe",
        Age:       30,
        Email:     "john.doe@example.com",
    }

    person2 := Person{
        FirstName: "Jane",
        LastName:  "Smith",
        Age:       25,
        Email:     "jane.smith@example.com",
    }
}
```

## Proper steps to initialize a Go project

One that took me some time but is not hard. You want to essentially create a new Directory:

Initializing a Go project is a straightforward process to be honest.

- Create a New Directory: Begin by creating a new directory for your project. You can do this with the following command:

```bash
mkdir newproject
cd newproject
```

- Replace "newproject" with the desired name for your project directory.

Initialize the Go Module: Use the go mod init command to initialize a Go module. This step defines the module's name, typically in the format github.com/yourusername/project_name. Replace "yourusername" with your GitHub username and "project_name" with the name of your project:

```bash
go mod init github.com/alfredosa/project_name
```

- Add Dependencies: If your project depends on external packages or libraries, use the go get command to fetch and add them to your project. Replace "blablabla" with the actual package or library you need:

```bash
go get blablabla
```

Repeat this step for each additional dependency your project requires.

This is the way. 

![this is the way](https://juststickers.in/wp-content/uploads/2021/01/this-is-the-way.png)

## Conclusion

I would write about pointers and addresses but I would like to dedicate more time to clearly lay out this topic. I am still confused by it sometimes. 

![Goodbye](https://i.imgflip.com/82mgwy.jpg)