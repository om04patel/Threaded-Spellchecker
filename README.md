# Threaded-Spellchecker
A multithreaded spellchecker program that runs tasks independently, synchronizes results, and updates shared resources using thread synchronization. Each task checks text files for spelling errors against a dictionary, outputs results to files, and summarizes the findings.

# Overview
This project is a multithreaded spellchecker program that allows you to start computationally intensive spell-checking tasks from a text-based menu. Each task runs in a separate worker thread, while the main thread handles user interaction. The program demonstrates the use of threads and thread synchronization in concurrent programming.

# Features

-Text-Based Menu: Start new spell-checking tasks or exit the program.

-Multithreading: Each spell-checking task runs in its own thread.

-Thread Synchronization: Proper synchronization to handle file writing and shared resources.

-Spellchecking: Checks text files against a provided dictionary and reports spelling mistakes.

# Task - Spellchecking

The spellchecking task takes two inputs: the text file to be checked and the dictionary file. The program then:

Counts the total number of spelling mistakes.

Identifies the three most frequently occurring misspelled words.

Saves the results to a file.

# Menu Options

Start a New Spellchecking Task:

Enter the dictionary file name.

Enter the input text file name.

Starts a new spellchecking task in a separate thread.

Exit:
Displays a summary of completed tasks and exits the program.

# Summary

On exit, the program provides a summary including:

Total number of files processed.

Total number of spelling errors encountered.

The three most common misspelled words and their frequencies.

# Command Line Argument

-l: If provided, the final summary is saved to a file instead of being displayed on the screen.

# Compilation and Execution

1.) git clone https://github.com/yourusername/Threaded-Spellchecker.git

2.) cd Threaded-Spellchecker

3.) make

4.) ./spellcheck

5.) ./spellcheck -l

# Output Format

Each spellchecker task outputs results to a file named username_A2.out in the format:

someFile.txt 32 someword badword2 badword3

The final summary includes:

Number of files processed: XXX

Number of spelling errors: YYY

Three most common misspellings: word1 (A times), word2 (B times), word3 (C times)

# Makefile Targets

spellcheck: Compiles the main executable.

clean: Deletes the executable and any temporary files.

# Known Issues

Ensure input files are accessible and valid.

Memory leaks may occur if the program exits while threads are still running.
