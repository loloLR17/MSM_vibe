# Décision POC — calcul global et pleine échelle

**FAISABILITÉ LIMITÉE.** Les six heures existantes supportent un calcul d'énergie
globale reproductible, même en présence de modulation. La comparabilité physique
et un résultat normatif de confort restent à démontrer. Aucun achat n'a été effectué.

## Ce que l'on sait calculer

MESURÉ sur les codes archivés, convertis au gain nominal non étalonné :

| Max RMS XYZ, hypothèse MCU | Médiane60s | P9560s | Maximum60s | Médiane120s |
|---|---:|---:|---:|---:|
| Poids plat 1–80Hz, mg | 31,14 | 36,79 | 51,41 | 31,44 |
| Filtre test LP10, mg | 7,24 | 12,51 | 16,17 | 7,28 |

Le LP10 réduit les fréquences élevées selon une formule publiée uniquement
pour tester le calcul. **Il ne représente pas une pondération humaine.** Le
rapport pondéré/plat varie entre0,206 et0,392 par minute ; on ne peut pas en
faire un facteur universel de conversion. Aucun seuil, score ou grade de confort.

Sur la période entière valide, RMS directionnel : 27,643mg plat, 7,313mg test.
Les distributions60/120s n'ont pas exactement la même population aux bords ;
182 paires alignées par méthode portent exactement sur le même contenu.
Leur recombinaison d'énergie est exacte à4,5e−16 relatif. Le maximum des deux
minutes peut dépasser le RMS120s de30% environ : une variation temporelle
attendue, pas une erreur de calcul. Rang des périodes : Spearman0,980 plat/
0,985 test entre maximum60s et RMS120s.

Contrôles : **10 tests ciblés et144 comparaisons réelles temporel/FFT PASS**,
écart relatif maximal2,81e−10 (tolérance0,001). La borne basse0,8/1/1,2Hz change
le RMS plat de moins de0,06% par minute sur cette archive. Cela ne prouve pas
la réponse physique à1Hz ; les basses fréquences contribuent peu ici. Sous
l'autre hypothèse de cadence, l'écart du plat atteint−0,873% ; aucune horloge SI.
Les bandes5–25 et25–80 portent en moyenne24,8% et75,0% de la somme des quatre
énergies de bandes. Ces filtres se chevauchent ; leur somme/total va de0,990 à1,117.

## Pourquoi ne pas passer directement à ±2g ?

MESURÉ sur sorties déjà filtrées/décimées : X [−0,932;+1,258]g,
Y [−1,259;+0,700]g, Z [−4,631;+7,677]g nominaux. Un épisode d'environ1,33s
contient41 sorties Z au-delà de2g, dont5 au-delà de4g. Sa cause physique n'est
pas connue. Même en tenant compte du dépassement possible du FIR, il impose
une entrée numérique contributrice d'au moins5,299g si la chaîne est correcte.
Les flags rails/clips sont nuls, sans garantie sur l'analogique ou les crêtes
natives non archivées. **Conserver ±8g sur le montage actuel.** ±4g n'est pas
un compromis automatiquement sûr, ±2g doit rester une future qualification dédiée.

SPÉCIFICATION FABRICANT : pas0,244/0,122/0,061mg pour±8/4/2g ; bornes d'arrondi
±0,122/0,061/0,0305mg. ST indique un bruit indépendant du FS, caractérisé à
100Hz–6,3kHz. Quatre fois plus de comptes ne promet pas quatre fois moins de
bruit à1Hz. Simulation HYPOTHÉTIQUE sur analogue connu à0,3mg sans bruit : biais
RMS plat +2,35% en8g contre−0,066% en2g ; ce gain dépend du signal et du bruit.
Elle ne recrée aucune information perdue dans l'archive.

## Avant d'acheter ou de comparer une nouvelle campagne

La faisabilité numérique ne justifie pas à elle seule une dépense ISO. Un achat
sera techniquement pertinent si l'indicateur humain visé et son édition sont
clarifiés et si l'on accepte de qualifier la chaîne : pose rigide, axes, gain,
réponse et bruit1–100Hz, cadence, température et transitoires. Il donnera la
définition normative manquante, sans corriger les limites du prototype.

Une courte mission physique séparée devra vérifier le FS par readback, mesurer
le bruit au repos et un signal de référence, conserver les bruts natifs autour
des événements et comparer les FS à pose/horloge identiques. Deux capteurs
synchrones sont préférables ; une alternance ne serait pas simultanée. Aucun
flash, debug, commande ou nouvelle acquisition dans cette mission. UTC/axes
navire/pose non qualifiés et réserve R3 sur digest RAM perdu sont maintenus.

[Figures et données](index.html) · [Preuves](README.md).
