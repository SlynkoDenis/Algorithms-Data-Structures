10.24
Reb-black tree written on C++.

Main.cpp is constructed in a way to check basic functions of tree, "char" is used as the default settings type of data in node. Write the name of .txt file while running the program if you want to create file with testing data. If you want to test the functioning of the tree it is necessary to enter to input stream nodes which are to be inserted in "key data" style (key is an integer number, excluding 0); enter "0 0" in order to end insertion; enter keys which are to be deleted in "key" style (before it is necessary to end insertion!); enter "0" in order to end deleting.


10.25
Update:

Structure of tree nodes was redone, now it is important to get pointer to data from the node. Some bags were fixed. Now main.cpp can either create test file only with keys or test the functioning of tree. Now it is necessary to write only "0" in order to end insertion. Basic types in tree are int, float and double for keys and char for data.


10.27
Update:

Bags were fixed. Now adapter.h header allows to work with the map as it is only set. Adapter_test.cpp tests functioning of set, input have to contain list of non-zero keys to be inserted with "0" as the flag of the end of insertion, list of non-zero keys to be checked for availability with "0" as the flag of the end of verification, list of non-zero keys to be deleted with "0" as the flag of the end of deleting. Attention: main.cpp works as before and generates tests adhering to the previous rules.


10.29
Update:

Bags were fixed. Now SearchTree class has the method for printing tree inorder. Now adapter_test.cpp works in three modes:
a) Enter "functioning_test" after the name of the program in terminal in order to check tree's main methods (testing data obeys the same rules as previously);
b) Enter "speed_test_custom_set" to measure SearchTree execution time on testing data built on the previous pattern;
c) Enter "speed_test_std_set" to measure std::set execution time on testing data built on the previous pattern.


10.30
Update:

Bags were fixed.