#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main()
{
    int parentToChild[2];
    int childToParent[2];

    int numbers[] = {10, 20, 30, 40, 50};
    int n = 5;

    pipe(parentToChild);
    pipe(childToParent);

    pid_t pid = fork();

    if (pid < 0)
    {
        cout << "Fork failed!" << endl;
        return 1;
    }
    if (pid > 0)
    {
        close(parentToChild[0]);
        close(childToParent[1]);
        write(parentToChild[1], &n, sizeof(n));
        write(parentToChild[1], numbers, sizeof(numbers));
        close(parentToChild[1]);
        int sum;
        read(childToParent[0], &sum, sizeof(sum));
        cout << "Parent: Sent integers: ";
        for (int i = 0; i < n; i++)
        {
            cout << numbers[i] << " ";
        }
        cout << endl;
        cout << "Parent: Received sum = " << sum << endl;
        close(childToParent[0]);
        wait(NULL);
    }
    else
    {
        close(parentToChild[1]);
        close(childToParent[0]);

        int count;
        int values[10];

        read(parentToChild[0], &count, sizeof(count));
        read(parentToChild[0], values, count * sizeof(int));

        close(parentToChild[0]);

        int sum = 0;

        for (int i = 0; i < count; i++)
        {
            sum += values[i];
        }

        cout << "Child: Calculated sum = " << sum << endl;

        write(childToParent[1], &sum, sizeof(sum));

        close(childToParent[1]);
    }

    return 0;
}