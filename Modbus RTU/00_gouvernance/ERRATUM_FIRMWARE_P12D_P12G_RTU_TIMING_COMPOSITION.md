# Erratum — P12-D / P12-G — composition du timing RTU

## Objet

Lors de la composition P12-H, une contradiction a été détectée entre la sémantique initiale du receiver portable P12-D et les événements temporels réellement produits par P12-G.

## Contradiction

P12-G émet, après le dernier octet reçu :

```text
BYTE -> 750 us -> SILENCE_T1_5 -> +1000 us -> SILENCE_T3_5
```

La version initiale de P12-D invalidait immédiatement une trame en réception à la réception de `SILENCE_T1_5`. Une trame normale devenait donc invalide avant l'événement `SILENCE_T3_5` et ne pouvait jamais être publiée.

## Correction normative

Le receiver distingue désormais l'attente de T3.5 :

```text
IDLE
  -> BYTE -> RECEIVING
  -> T1.5 -> WAITING_T3_5
       -> T3.5 -> publication
       -> BYTE avant T3.5 -> INVALID -> rejet à T3.5
```

Ainsi, atteindre T1.5 après le dernier octet n'est pas en soi une erreur. En revanche, recevoir un nouvel octet après T1.5 et avant T3.5 invalide la trame.

## Portée

Le freeze P12-D initial est supersédé uniquement pour la transition liée à T1.5. Les autres règles P12-D restent inchangées. P12-G reste inchangé.

Les tests verrouillent :
- `BYTE... -> T1.5 -> T3.5` : publication ;
- `BYTE... -> T1.5 -> BYTE -> T3.5` : rejet.

## Validation

- Host : VALIDATED.
- STM32 Cortex-M33 cross-build : VALIDATED.
- Hardware : PENDING.
