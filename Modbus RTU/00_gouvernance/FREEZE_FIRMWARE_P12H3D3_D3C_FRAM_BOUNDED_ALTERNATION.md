# FREEZE — Firmware P12-H3d3-D3-C — Alternance transactionnelle physique bornée FRAM

## Statut

**GELÉ — VALIDATION PHYSIQUE ACQUISE**

D3-C clôt la campagne de persistance physique H3d3 par une séquence bornée de commits H3d2 normaux sur la FRAM réelle du STM32U575.

## Baseline physique d'entrée

Baseline récupérée et observée avant armement :

- recovery : `VALID`
- génération : `7`
- image active : `A`
- payload[0] : `0xA9`

Le harnais D3-C était strictement gardé sur cette baseline.

## Scénario qualifié

Le harnais exécute exactement trois commits transactionnels normaux, sans formatage, mécanisme de réparation ni retry :

1. mutation `0xAA`
2. mutation `0xAB`
3. mutation `0xAC`

Chaque commit est suivi d'un recovery H3d2 et de l'enregistrement des sondes de génération, image active, superblock actif et valeur récupérée.

## Résultats physiques observés

Les sondes GDB ont donné :

| Étape | commit | recovery | génération | image | superblock | valeur |
|---|---:|---:|---:|---:|---:|---:|
| 1 | `TR2_OK` | `TR2_OK` | 8 | B (1) | B (1) | `0xAA` |
| 2 | `TR2_OK` | `TR2_OK` | 9 | A (0) | A (0) | `0xAB` |
| 3 | `TR2_OK` | `TR2_OK` | 10 | B (1) | B (1) | `0xAC` |

Sondes globales :

- `tr2_fram_d3c_attempted = 1`
- `tr2_fram_d3c_completed = 1`
- trois résultats de commit à `TR2_OK`
- trois résultats de recovery à `TR2_OK`

La campagne démontre physiquement, sur cette séquence bornée :

- génération strictement monotone : 7 → 8 → 9 → 10 ;
- alternance image : A → B → A → B ;
- alternance publication/superblock : A → B → A → B ;
- readback cohérent de chaque mutation ;
- absence de formatage, réparation et retry dans le scénario.

## Désarmement et recovery final

Après acquisition des preuves, le gate D3-C a été remis à zéro.

Commit de désarmement :

`93e8fc764dd1544d55d96e350dac43af1217c08a`

Le firmware désarmé a ensuite :

1. passé la validation complète locale ;
2. été flashé sur la NUCLEO-U575ZI-Q ;
3. repris le clignotement normal LD1 ;
4. effectué un recovery non destructif après redémarrage.

Résultat final observé :

- recovery : `VALID` (`0x1`)
- génération : `10` (`0xA`)
- image active : `B` (`0x1`)
- payload[0] : `0xAC`

Cette vérification confirme la persistance de la dernière autorité après désarmement et redémarrage normal.

## Incidents de harnais pendant la préparation

Deux défauts de placement du harnais D3-C ont été détectés avant toute écriture D3-C :

1. bloc initial placé avant la définition du gate, donc éliminé du firmware effectif ;
2. correction intermédiaire placée avant le recovery H3d2, donc garde jamais franchie.

Dans les deux cas, les sondes ont confirmé `attempted = 0` et la baseline physique est restée `VALID / gen7 / A / 0xA9`.

Correction finale du placement :

`294180a8b6ba2601d6181fd63c0f4db341e409ff`

Ces incidents ne constituent pas des essais D3-C valides et n'ont pas modifié la FRAM.

## Limites explicites

Ce gel qualifie uniquement la séquence physique bornée définie ci-dessus. Il ne constitue pas :

- un essai d'endurance FRAM ;
- une qualification brownout/ramp ;
- une qualification de coupure arbitraire au milieu d'une transaction SPI ;
- une injection de fautes SPI ;
- une caractérisation des performances en exploitation vibratoire ;
- une extension des garanties déjà gelées H3d2/D2-D.

Les propriétés de coupure d'alimentation physique restent celles qualifiées séparément par P12-H3d3-D2-D.

## Conclusion

**P12-H3d3-D3-C est physiquement qualifié et gelé.**

La baseline physique de sortie pour la suite est :

`VALID / génération 10 / image B / payload[0] = 0xAC`.
