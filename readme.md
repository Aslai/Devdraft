__Devdraft Numbers Game submission__
===================================================================


-------------------------------------------------------------------
__Background __
-------------------------------------------------------------------
I lurk on Craigslist every day looking for potential job openings 
for someone as green as myself. There was an ad for Devdraft, and 
it caught my eye. I usually enjoy solving puzzles on the 
Code Golf / Puzzles Stackexchange site, so I figured I would try 
participating in this event.

-------------------------------------------------------------------
__General Information About the Solution__
-------------------------------------------------------------------
I spent many hours poring over various inputs and outputs from the 
algorithm presented in the challenge. I ultimately discovered some 
deterministic patterns that can be used to determine the solution 
to the problem based on the differences between the integers 
provided for a given puzzle. I must say that I was pleasantly 
surprised when 100 digit numbers were being processed in 
nanoseconds.

-------------------------------------------------------------------
__My Thought and Work Process__
-------------------------------------------------------------------
##Starting Off##

The first thing I thought about was how I would represent numbers as 
large as 10^100. I'd never written a big number object like this 
before, so it was an interesting exercise. Rather than following the 
obvious approach of operating on strings, I decided to essentially 
use 32 bit integers as base 4,294,967,296 digits. It was a little 
difficult to conceptualize how I would handle things like carrying 
at first, but because of my chosen approach, I was able to lean 
heavily on standard unsigned integer arithmetic to help me out. 
Feel free to look at the ``Number`` class in my code for 
implementation details. It's not the prettiest, but it's fast and 
memory efficient.

Once I actually had a big number class written out, I figured that 
since this challenge is asking for solutions to numbers as large as 
10^100, there had to be some mathematical trick that I could 
leverage to speed up calculations. I started out by getting a 
naïve solution up and running just so I could see what speed 
restraints I would be dealing with. It wasn't pretty. 

The solution I whipped up was a simple depth-first decision tree 
crawler that ran slow as molasses. even just 10 digit numbers would 
take longer to process than my patience would allow. Once I had the
naïve solution up and running though, I could quickly run reams of 
small numbers through it (It might be slow, but it's still thousands
of times faster than doing it by hand). 

-------------------------------------------------------------------
##A Very Handy Simplification##

Very early on in the process, I realized that the three number input
can be boiled down to to something even more basic. Since the game 
just involves splitting the difference between the three numbers 
provided, I realized that I could take the difference of the three 
numbers, discard the three numbers, and yet lose no necessary 
information in the process. This removed an entire input from the 
function I was being asked to replicate, reducing the data set to 
a two dimensional one (from three dimensions). That saved a lot of 
headache. 

From there, I tried for hours trying to somehow relate the numbers 
I was getting to a pure mathematical function. One thing that really
stuck out to me was that rounds that you can force the game from the
second move tended to have a lot of primes in the numbers. 99 times 
out of 100, numbers that favored a second move had multiple primes 
in the numbers I was looking at. I couldn't help but shake the idea 
of the potential significance of this. In a challenge like the one 
presented, coming up with an efficient way to find and verify primes
can always be a real time sink. 

-------------------------------------------------------------------
##Time for some Serious Examination##

Later, I decided to just run as much organized data as I could 
through the solution I had made. Once I had 10000 elements to look 
at, (spanning the lowest 100x100 units of the plane of possible 
inputs) I noticed just how rare it is to be able to force a game 
from the second move. In my sample set, the chance of it happening 
was about 12%. I noticed something very peculiar though; when it 
_did_ happen that the second move was advantageous, all of the 
surrounding data points also showed the same second move advantage. 
There would be hundreds of lines where the first move was the only 
winning move, and then there would be a clump 20 (mostly) 
consecutive data points where the opposite is true.

I decided to look closer at the data for just the second move 
advantage numbers, so I just removed all of the first move advantage
data. I figured that if I could predict which rounds I should take 
the second turn on (which is only a tenth of the possible rounds in 
my sample set) with 100% accuracy, then I could then just assume all 
the other rounds need you to take the first move. When I looked at 
this data all by itself, there was something that jumped out at me 
immediately. There were large gaps between consecutive chunks of 
data. Not only that, but where the gaps ended were prime numbers! 
Chunks of data started at 1, 3, 11, and 43. I spent quite some time
trying to figure out what determined the length of the chunks and
gaps, until I finally realized that each chunk was as long as the 
value of the first number in that chunk. By that, I mean that the 
chunk of data with 11 as the first number in the set was 11 numbers
long. I figured it would break down with the chunk starting at 43,
but it held there, too.

With this new information, I started the program back up, only this 
time I had it crunch the lower 1000x1000 unit dimension to see if I
could get the next number in this sequence. While that was chugging 
along, I spent far too long looking at a table of prime numbers 
trying to relate the sequence to itself. It eventually proved 
fruitless, especially with my limited knowledge of the sequence. It
was going to take about an hour to crunch all the data I wanted the 
program to take care of, so I took a relaxing DotA break.

-------------------------------------------------------------------
##The Info I was Looking for All Along##

When the task finally finished, I eagerly began to thumb through the 
data it had produced. The next numbers in the sequence were... 171 
and 683. There went the prime theory. At this point, I was getting 
weary. It was midnight, I had been awake for 20 hours, and I was 
getting frustrated at my hazy brain. I turned to my friends to see 
if they had any insight as to what relation these numbers might 
have, and one of them pointed out that it's just ``z3=z2+4*(z2-z1)``,
where ``z2`` is some element in the series, ``z1`` is the element 
before it, and ``z3`` is the element after it. I had spent so much 
time trying to be clever with logarithms and primes and exponents 
that it didn't even cross my mind to do a simple operation like that.
This was the breakthrough I needed though; with this I had mountains
of new-found motivation to stay up through the night working on it 
to completion. 

After adding a subtraction function to my ``Number`` class, I set up
a function to build a lookup table for the pattern I found in the 
data, and a speedy function for comparing game numbers to the table.
The result of this work can be found in the ``CheckMovesFast`` 
function. It's nothing that fancy when you are looking at it, but I 
did quite a bit of what essentially amounted to black box reverse 
engineering. Sure, I built the black box from a spec, but as far as 
I can tell, the method I used for the fast version of the function 
is so far removed from the specification that it's hard to tell that
it performs such a specific and relatively complex function.

-------------------------------------------------------------------
__Conclusion__
-------------------------------------------------------------------

This was a very fun activity I got to participate in. The skills I 
had to exercise were wildly different from what I was expecting to 
use, though. Who on earth would have thought I would be black box 
reverse engineering code that I literally just wrote hours prior? 
The various red herrings were interesting as well. It's very easy to
fall into traps where the you excuse discrepancies in the patterns 
you're seeing due to confirmation bias. I also feel really weird
having written code that appears to work, but I honestly don't know 
if it's 100% correct. I ran it through various test cases as 
rigorously as I could with my naïve implementation, but what if even
my naïve implementation was partially incorrect? What if the pattern
holds all the way up to 1000x1000 (which is the furthest extent I 
tested it to) but breaks down at 1001x1001? This is the most 
ambivalent I've ever been about software I've written.

Hopefully I manage to collect all the points with this crazy piece 
of code!

-------------------------------------------------------------------



_September 16th, 2014_

