# Gel P12-H3d3-D3-B — Caractérisation temporelle physique FRAM

## Statut

**GELÉ — VALIDATION PHYSIQUE ACQUISE**

D3-B caractérise les durées observées sur la FRAM réelle du montage STM32, sans introduire de seuil de performance ni modifier le protocole transactionnel H3d2.

## 1. Baseline et méthode

La campagne finale a été exécutée depuis une autorité explicitement observée :

- recovery : `TR2_OK` ;
- état : `VALID` ;
- génération : 6 ;
- image active : B ;
- `payload[0] = 0xA8`.

Instrumentation : `HAL_GetTick()`, résolution 1 ms. Les mesures sont descriptives. Une mesure de 0 ms signifie donc « inférieure à la résolution de 1 ms », et non durée physique nulle.

Le backend STM32 FRAM reste en SPI à 2,5 MHz et conserve son découpage de transfert existant. Aucun retry, formatage ou mécanisme de réparation n'a été ajouté.

## 2. Résultats physiques observés

| Opération | Résultat | Durée observée |
|---|---:|---:|
| lecture 64 B | `TR2_OK` | 0 ms (< 1 ms de résolution) |
| écriture 64 B | `TR2_OK` | 1 ms |
| lecture 51 818 B | `TR2_OK` | 167 ms |
| écriture 51 818 B | `TR2_OK` | 167 ms |
| commit H3d2 complet | `TR2_OK` | 851 ms |

Le commit complet inclut les opérations transactionnelles H3d2 existantes ; cette mesure ne constitue pas un seuil contractuel de performance.

## 3. Résultat transactionnel associé

Les sondes de qualification ont donné :

- `attempted = 1` ;
- toutes les opérations physiques mesurées : `TR2_OK` ;
- `commit_result = TR2_OK` ;
- recovery post-commit : `TR2_OK`, `VALID`, génération 7, image A ;
- `payload[0] = 0xA9` ;
- `completed = 1`.

La transition physique observée est donc :

`VALID / gen6 / image B / 0xA8 -> VALID / gen7 / image A / 0xA9`.

## 4. Retour à l'état désarmé

Après observation des sondes, la porte `TR2_FRAM_D3B_ALLOW_TIMING` a été remise à `0U`.

Le firmware désarmé a ensuite été :

- compilé et validé par la chaîne complète `tr2_validate.sh` ;
- flashé sur la NUCLEO ;
- observé avec LD1 en clignotement normal.

Le recovery non destructif après ce redémarrage a confirmé :

- état `VALID` ;
- génération 7 ;
- image active A ;
- `payload[0] = 0xA9`.

Cette autorité constitue la baseline physique d'entrée de D3-C.

## 5. Incidents de campagne conservés

Deux exécutions antérieures avaient effectivement fait progresser la persistance mais leurs sondes temporelles avaient été perdues après reset/reflash avant lecture. Elles ne sont pas utilisées comme mesures D3-B.

Une tentative suivante n'est pas entrée dans le harnais car le garde gen6 contenait encore `active_image == 0U` alors que l'autorité observée était B (`1U`). Les sondes avaient confirmé `attempted = 0` et aucune mutation FRAM. Le garde a été corrigé avant la campagne finale.

Ces incidents n'altèrent pas les valeurs de la campagne finale, observées en RAM au point d'arrêt volontaire avant tout reset.

## 6. Limites

D3-B ne qualifie pas :

- stack high-water ;
- performances avec acquisition vibration de production active ;
- DMA ou optimisation SPI ;
- comportement temporel sous brown-out ;
- endurance FRAM ;
- seuil contractuel de temps de commit.

Le comportement/durée des timeouts demandé dans le cadrage D3 reste non caractérisé par cette campagne de succès et ne doit pas être présenté comme acquis.

## 7. Décision

**P12-H3d3-D3-B est gelé pour la caractérisation temporelle physique réussie.**

La suite est **D3-C — alternance bornée**, à partir de la baseline observée `VALID / gen7 / image A / payload[0]=0xA9`, sans formatage ni réparation.
