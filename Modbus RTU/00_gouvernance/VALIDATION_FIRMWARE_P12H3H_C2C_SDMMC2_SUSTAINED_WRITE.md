# P12-H3h-C2c — Caractérisation soutenue SDMMC2 1-bit / 10 MHz

## Objet

Caractériser l'écriture brute sur une séquence plus longue que C2b, sans changer
le chemin physique : SDMMC2 1-bit / 10 MHz, média de test sacrifiable, sans
filesystem, format ni erase explicite.

C2c écrit huit chunks contigus de 64 KiB, attend le retour à TRANSFER après chaque
chunk, mesure chaque durée et la durée globale, puis relit et compare les 512 KiB.

## Baseline firmware testée

- Commit : `3b0b9089bd4954929311b695d1bce54d69353bb4`
- Message : `Firmware: characterize sustained SDMMC2 writes`
- Bloc initial : 2048
- 8 chunks x 128 blocs x 512 B
- Volume total : 524 288 B
- Largeur : 1 bit
- ClockDiv : 8, chemin 10 MHz

## Résultat physique global

| Variable | Valeur |
|---|---:|
| `tr2_sdmmc2_stage` | 14 |
| chunks écrits | 8 |
| durée globale | 436 ms |
| octets écrits | 524 288 B |
| débit global calculé | 1 202 495 B/s |
| état final carte | 4 / TRANSFER |
| statut relecture | 0 / HAL_OK |
| chunks vérifiés | 8 |
| mismatches | 0 |
| premier mismatch | 0xffffffff |
| code erreur | 0x0 |

## Distribution par chunk

Durées, en ms :

```text
55, 55, 54, 55, 54, 54, 54, 55
```

Débits calculés, en B/s :

```text
1 191 563, 1 191 563, 1 213 629, 1 191 563,
1 213 629, 1 213 629, 1 213 629, 1 191 563
```

La dispersion observée sur cette séquence est donc seulement de 1 ms entre chunks.

## Confrontation au contrat bulk

Débit logique gelé P8-B :

```text
426 672 B/s
```

Rapport débit global C2c / débit logique :

```text
1 202 495 / 426 672 = 2,818
```

Le débit global mesuré représente environ 2,82 fois le débit logique requis, soit
une marge moyenne d'environ +182 % sur cette séquence de 512 KiB.

## Mise en perspective avec C2b

C2b avait mesuré 65 536 B en 298 ms, soit 219 919 B/s, avec intégrité parfaite.
C2c mesure ensuite huit chunks contigus de même taille en 54–55 ms chacun.

Les observations physiques établissent donc simultanément :

1. le chemin 1-bit / 10 MHz peut fournir, sur C2c, un débit moyen très supérieur
   au flux logique TR2 ;
2. une latence ponctuelle de 298 ms a déjà été observée sur une écriture de 64 KiB ;
3. l'intégrité écriture/relecture est validée sur C2b et C2c.

C2c ne permet pas d'affirmer que 298 ms est une valeur maximale ni d'établir une
borne de pire cas microSD.

## Conclusion bornée

**H3h-C2c valide la capacité de débit moyen du chemin SDMMC2 1-bit / 10 MHz pour
le flux logique bulk de 426 672 B/s sur la séquence physique testée.**

Le mode 4-bit n'est pas requis pour satisfaire le débit moyen observé et reste hors
du chemin critique.

La latence d'écriture variable reste en revanche une contrainte de conception du
backend : H3h-D devra découpler l'acquisition de l'écriture microSD par buffering
et ne devra pas dimensionner ce buffering uniquement sur le débit moyen C2c.

La qualification de production devra traiter explicitement les latences longues,
la durée de campagne et les scénarios de checkpoint/power-loss conformément au
contrat bulk gelé.
