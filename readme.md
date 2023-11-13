<img align="right" src="https://cdn-icons-png.flaticon.com/512/6124/6124995.png" width="200">

# Labyrinth-Tux
Labyrinth Game for Linux, with a server/client architecture.
> This game was made as an university project

<br>

## Features 📑
* Motor: The application responsible for the game management and responding to all the players requests and in-game actions
* JogoUI: The application that all players use to play the game
###### No JogoUI can be initialized without the Motor being ON

<br>

## Requirements 📋
You'll need <a href="https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/" >ncurses</a> installed on your system
```
  sudo apt install libncurses5 libncurses5-dev
```
<br>


Also it is required to have 4 ambient variables:
* `INSCRICAO` : time gap were the players can enter to play (in seconds)
* `NPLAYERS` : minimum number of players needed to start the game
* `DURACAO` : base level time (in seconds)
* `DECREMENTO` : time reduced on each level (in seconds)

<br>

## How to Use 🛠
In the terminal use the following command: 
<br>
`make all` : to initialize all the `.exe` files needed
<br>
`make clear` : This will clear all the temporary files and the `.exe` files
<br><br>
`./motor` : to initialize the motor application
<br>
`./jogoUI <username>` : to initialize the jogoUI application
<br>
<br>

## Commands 💻 (Motor)
* `users` : prints all the users currently online
* `kick <username>` : kicks the player
* `bots` : lists all the bots
* `bmov` : adds an dynamic wall
* `rbm` : removes one dynamic wall
* `begin` : start the game
* `settings` : prints the system settings
* `map <level>` : prints the map and settings of one level 
* `help` : prints a command list
* `end` :  closes the motor and kicks all the players
<br><br>

## Commands 👥 (JogoUI)
* `UP ARROW` : moves upwards
* `DOWN ARROW` : moves downwards
* `RIGHT ARROW` : moves to the right
* `LEFT ARROW` : moves to the left
* `SPACE KEY` : changes to writing mode
* `msg <username>` : sends an message to a player
* `help` : prints a command list
* `exit` :  closes the jogoUI and notifies the motor
<br><br>
