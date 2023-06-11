/*let i = 500;
let j = 1;
let k = 0;
let intervalId = 0;

function test() {
    i -= 1;
    println(`Function with iteration ${i}`);
    if (i > 0) {
        let id = setTimeout(test, i);
        let id2 = setImmediate(immediateTest);
        if (j > 10) {
            clearTimeout(id);
            clearImmediate(id2);
            intervalId = setInterval(helloRepeatedly, 1000);
        }
    }
}

function immediateTest() {
    println(`Immediate function with iteration ${j}`);
    j++;
}

function helloRepeatedly() {
    println("Hello");
    k++;
    if (k == 10) {
        println("Goodbye awe");
        clearInterval(intervalId);
    }
}

setTimeout(test, i);
setImmediate(immediateTest);
*/
// sleep test

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

async function test() {
    println("Hello");
    await sleep(1000);
    println("Goodbye");
}

test();

setTimeout(async () => {
    await sleep(500);
    println("Hello");
}, 10);
