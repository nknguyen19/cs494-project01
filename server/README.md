# Client Request Formats
## New Player Registering
```
REGISTER <player_nickname>
```

### Reponse Formats
```
200 Nickname registered successfully.
400 Nickname is already taken. Please try again.
400 Nickname is invalid. Please try again.
400 You've already registered in a game.
```

## In-Game Requests
### Server Common Response for In-Game Requests (ANSWER, MOVE)
```
400 You must register first.
400 The game has not started.
400 Not your turn.
```

### Answering
```
ANSWER <answer_option>
```
`answer_option` must be A, B, C or D (case insensitive).

#### Response Formats
```
200 CORRECT.
200 INCORRECT.
400 Invalid answer. Answer must be A, B, C or D (case insensitive).
```

### Moving Turn to the Next Player
```
MOVE
```
Note that whatever value comes after MOVE command code will be ignored.

#### Response Formats
```
200 Move turn sucessfully.
400 You cannot move turn anymore.
```

### Logging Out From a Game
```
LOGOUT
```
Note that whatever value comes after MOVE command code will be ignored.

#### Response Formats
```
200 Logout successfully.
400 You haven't registered to any game.
```

# Server Message Formats
## Game status Updating
```
<game status>
<number of player in game>
<total number of player>
<player_1_nickname> <player_1_status> <player_1_can_move_turn>
...
<player_n_nickname> <player_n_status> <player_n_can_move_turn>
<number of questions>
<current question index>
<current question>
<option_A>
<option_B>
<option_C>
<option_D>
```