using System;
using System.IO;
using System.Text.RegularExpressions;

namespace ResultsCounter
{
    class Program
    {
        static void Main(string[] args)
        {
            string fileAppend = "m++KEEP.txt";
            string[] lines = File.ReadAllLines("E:/Documents/CompSci Yr 3/Concurrent/ConcurrentAssignment/" + fileAppend);
            Console.WriteLine(fileAppend);
            double count = 0;
            double time = 0;
            for (int i = 1; i < 101; i += 2)
            {
                Match match = Regex.Match(lines[i], "a ([0-9]+) speedup");
                int num = Convert.ToInt32(match.Groups[1].Value);
                count += num;
                match = Regex.Match(lines[i], "time: ([0-9]+) microseconds");
                num = Convert.ToInt32(match.Groups[1].Value);
                time += num;
            }
            // The code provided will print ‘Hello World’ to the console.
            // Press Ctrl+F5 (or go to Debug > Start Without Debugging) to run your app.
            Console.WriteLine("{0}, {1}", count / 50, time / 50);

            count = 0;
            time = 0;
            for (int i = 104; i < 204; i += 2)
            {
                Match match = Regex.Match(lines[i], "a ([0-9]+) speedup");
                int num = Convert.ToInt32(match.Groups[1].Value);
                count += num;
                match = Regex.Match(lines[i], "time: ([0-9]+) microseconds");
                num = Convert.ToInt32(match.Groups[1].Value);
                time += num;
            }
            // The code provided will print ‘Hello World’ to the console.
            // Press Ctrl+F5 (or go to Debug > Start Without Debugging) to run your app.
            Console.WriteLine("{0}, {1}", count / 50, time / 50);

            count = 0;
            time = 0;
            for (int i = 207; i < 307; i += 2)
            {
                Match match = Regex.Match(lines[i], "a ([0-9]+) speedup");
                int num = Convert.ToInt32(match.Groups[1].Value);
                count += num;
                match = Regex.Match(lines[i], "time: ([0-9]+) microseconds");
                num = Convert.ToInt32(match.Groups[1].Value);
                time += num;
            }
            // The code provided will print ‘Hello World’ to the console.
            // Press Ctrl+F5 (or go to Debug > Start Without Debugging) to run your app.
            Console.WriteLine("{0}, {1}", count / 50, time / 50);

            // Go to http://aka.ms/dotnet-get-started-console to continue learning how to build a console app!
        }
    }
}
