
//getElementById uses the id='' 
//getElemenyByClass uses the class=''

//querySelector for first css element
//use . for classes, # for ids 
const basicTextChange = document.querySelector('.test')
const numbersList = document.getElementById('numbers')
const randomNum = document.getElementById('num')
const family = document.querySelector('.Family')

basicTextChange.innerText = 'test';

const numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
const people =  [
    { name: "Cameron", age: 21, car: "Honda"},
    { name: "Connor", age: 17, car: "Lexus"},
    { name: "Katie", age: 20, car: "Tesla"},
    { name: "Vanessa", age: 24, car: "Honda"},
    { name: "John", age: 23, car: "idk"}
];

numbers.forEach(num => {
    const div = document.createElement('div')
    div.innerText = num
    numbersList.appendChild(div);
})

function callRand() {
    randomNum.innerText = numbers[Math.floor(Math.random() * numbers.length)]
}

//array functions 

//map will apply an operation to each element and return a new list
const namesOnly = people.map(n => n.name)
const nameDiv = document.createElement('div')

// join will insert a character between each element in an array
nameDiv.innerText = namesOnly.join(" | ")
family.append(nameDiv);

//filter
//Purpose: Keep only the items that match a condition.


