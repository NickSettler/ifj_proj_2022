# Getting started

1. Clone the project 
2. Open the task in task.pdf
3. Start working with `*.c` files according to the task

## Lexical analyzer

The following diagram shows the structure of the finite state machine for the lexical analyzer.

```mermaid
stateDiagram-v2
    [*] --> Start
    Start --> KeywordState: ? / a..z
    Start --> IdentifierState: $
    Start --> IntegerState: 0..9
    Start --> "="
    Start --> ";"
    
    state KeywordState {
        [*] --> Keyword: a..z
        Keyword --> Keyword: a..z
        Keyword --> [*]
    }
    
    state IdentifierState {
        [*] --> Identifier: A..z, _
        Identifier --> Identifier: A..z, 0..9, _
        Identifier --> [*]
    }
    
    state IntegerState {
        [*] --> Integer: 0..9
        Integer --> Integer: 0..9
        Integer --> [*]
    }
    
    IntegerState --> FloatState: .
    
    state FloatState {
        [*] --> Float: 0..9
        Float --> Float: 0..9
        Float --> [*]
    }
    
    "=" --> [*]
    ";" --> [*]
```