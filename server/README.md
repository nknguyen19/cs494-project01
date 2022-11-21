# Client Request Formats
## New Player Registering
```
REGISTER <player_nickname>
```

### Reponse Formats
```
200 Nickname registered successfully.
```

```
400 Nickname is already taken. Please try again.
```

```
400 Nickname is invalid. Please try again.
```

## Answering
```
ANSWER <answer_option>
```
`answer_option` must be A, B, C or D (case insensitive).

### Response Formats
```
200 Correct answer. Continue answering.
200 Incorrect answer. You've been disqualified.
400 You must register first.
400 The game has not started.
400 Not your turn.
400 Invalid answer. Answer must be A, B, C or D (case insensitive).
```

## Moving Turn to the Next Player
```
MOVE
```

### Response Formats
```
200 Move turn sucessfully.
400 You must register first.
400 The game has not started.
400 Not your turn.
400 You've already move your turn once.
```

# Server Message Formats
## Game State Updating
```
<game_state>
<no_players>
<max_no_players>
<player_1_nickname> <player_1_status> <player_1_can_move_turn>
...
<player_n_nickname> <player_n_status> <player_n_can_move_turn>
<current_question>
<answer_A>
<answer_B>
<answer_C>
<answer_D>
```