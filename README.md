# LanJudge

This is the REST API for the services provided by LanJudge. The server is written in GoLang and the main code execution engine is written in C language.

## Why LanJudge?

Our aim is to create a local online judge service which could be used by our college, and eventually in many college labs for the conduction of lab exams in a more organized and objective way.

> **An online judge is an online system to test programs in  programming contests.**

We want LanJudge to be focused around conduction of lab exams in colleges and schools, allowing the administrator to set questions and the expected outputs in the admin facing application.

 A timed contest is created during which the students, each with their own accounts can compile and submit their codes, which are automatically graded and sent back to the admin.


 ## Components in LanJudge
 - **Frontend :** We'll need the two following applications:
   - Admin facing application : a place to create and manage contests, and maintain a record of the grades of participants.
   - Student facing application : where a student can join a contest, write, compile and submit their code.
  <br>
  <br>

 - **Backend :** A RESTful end-to-end API which can respond to requests sent from the frontend, both admin and student requests.  Following are the subcomponents in backend.
   - Engine : this is the piece of executable code which will actually compile and execute the code.
   - Database : a database is essential to store both admin facing and user data, like codefiles, grades and contest details.


## Our approach in designing the Judge

The most important component of this project is the core, engine.c which has the heavy task of actually compiling and executing a user code. We have decided to do this with C language using the rich unix libraries.

A code can be compiled and executed just how we do it in a terminal, for example using gcc to execute a C code. Writing this command inside the `exec` system call after using `fork`  gives us the power to run the code as a process.


**How do we contain the application in such a way that no malicious code slips in?**

 We understand that access to most of the system calls has to be blocked, and only the necessary ones should be able to run from the user code. We are isolating these system calls to whitelist them, which can be done by using `ptrace` to see which system calls are actually being called in the main function.

 ## Current Progress

- [X] A basic engine to compile a C code. 
- [X] Making the engine capable of executing a C code.
- [X] Allowing custom inputs and outputs to the code.
- [ ] Generate a list of system calls to whitelist.
- [ ] Rudimentary Go Server to take a code and return its output.
- [ ] Setting up the database and options to create contests.
- [ ] Create admin facing and student applications.
- [ ] Deploy and maintain