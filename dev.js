

let answer;

res = async function() {
    const readline = require('node:readline/promises').createInterface({ input: process.stdin, output: process.stdout, });

    answer = await readline.question('What do you think of Node.js? ');

    console.log(`Thank you for your valuable feedback: ${answer}`);

    readline.close();
}()

console.log(`Answer ${answer}`)


for (let i = 0; i < 10; i++) {
    console.log(i)
}