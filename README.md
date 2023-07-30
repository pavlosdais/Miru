<p align="center">
  <img src="logo/miru_logo.png">
</p>

# Miru
Miru is a [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface) chess engine written in C++. Miru in itself is not a complete chess program and requires a UCI-compatible graphical user interface (eg. [Arena](http://www.playwitharena.de/)). It's important to note that the engine is still under development so improvements & potential bug fixes are to be expected.

## Features
Huge inspiration has been taken from tutorials, stronger engines and external research. That being said, here are some of the features used:

### Board Representation
- [Bitboards](https://www.chessprogramming.org/Bitboards)

### Search
- [Negamax](https://www.chessprogramming.org/Negamax)
  - [Principal variation search](https://www.chessprogramming.org/Principal_Variation_Search)
- [Quiescence](https://www.chessprogramming.org/Quiescence_Search)
- [Aspiration Windows](https://www.chessprogramming.org/Aspiration_Windows)
- [Null nove pruning](https://www.chessprogramming.org/Null_Move_Pruning)
- [Razoring](https://www.chessprogramming.org/Razoring)
- [Late move reduction](https://www.chessprogramming.org/Late_Move_Reductions)
- [MVV-LVA](https://www.chessprogramming.org/MVV-LVA)
- [Hash move/ PV/ killer/ history move ordering](https://www.chessprogramming.org/Move_Ordering)
- [Transposition table](https://www.chessprogramming.org/Transposition_Table)

### Evaluation
- [Material](https://www.chessprogramming.org/Material)
- [Piece-Square Tables](https://www.chessprogramming.org/Piece-Square_Tables)
- [Pawn Structure](https://www.chessprogramming.org/Pawn_Structure)

## Strength
The engine is estimated to be roughly at around 2000 elo, though, it still needs to be officially tested against other engines.

## Compilation and usage
The engine can be compiled using `make` *or* `make final` for the final release. After compiling you can use the engine either </br>
- *Manually*, by running `make run` and following the UCI protocol [here](https://www.wbec-ridderkerk.nl/html/UCIProtocol.html). </br>
- *Automatically*, by loading it into a GUI.
