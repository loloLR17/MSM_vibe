# Écart SPI3 confirmé — proposition logique, aucune retouche à exécuter

Mission TR2-20261117-SHIELD-DUAL-BRINGUP. L'audit porte sur le PCB V2.3 existant,
conservé inchangé. Ce document prépare une décision corrective ; il ne libère
aucun CAM et n'autorise pas une découpe ou un pontage immédiat.

## Faits recoupés

L'utilisateur confirme MKIGIBV2 (V3 était une faute de frappe). Sur001 hors tension :
masse J2-11/TP2=0,1 Ω ; J2-6/TP3=0,3 Ω ; TP3/SCL≈600 kΩ ; **TP3/SDO=0,3 Ω**.
Les quatre liaisons SPI demandées ne présentent pas la continuité prévue.

Le schéma **ST DB4012 Rev3, page2 figure2**, rendu et inspecté, donne le brochage
ci-dessous. [Référence ST](https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf).
Le téléchargement direct ST a échoué ; le document ST a été récupéré depuis un
miroir Xon, dont URL/hash sont consignés dans le manifeste. Ce miroir n'est pas
présenté comme un hébergement officiel. Les tableaux web ont servi au recoupement,
pas à remplacer la lecture du dessin.

La lecture native KiCad du fichier PCB actuel confirme ces quatre écarts :

| MCU / entrée shield | Fonction voulue | Contact J8 actuel | Fonction ST sur ce contact | Contact requis |
|---|---|---|---|---|
| PC9 / J2-4 | CS |21| SDA/MOSI |19/CS |
| PC10 / J2-6 / TP3 | SCK |22| SDO/MISO |20/SCL |
| PC11 / J2-8 | MISO |24| NC |22/SDO |
| PC12 / J2-10 | MOSI |23| NC |21/SDA |

Les contacts19/20 sont actuellement NC dans notre PCB ;23/24 le sont sur
l'adaptateur ST. L'erreur documentaire et de conception est établie. Le raccord
horloge→SDO est aussi confirmé sur le montage001 par les résistances. Ne pas en
déduire que le capteur est sain ou que tous les contacts de002 ont été mesurés.

Les contrôles ERC/DRC antérieurs validaient la cohérence de nos propres nets ;
ils ne prouvaient pas la correspondance du brochage constructeur. Leur succès
n'est pas annulé rétrospectivement, mais sa portée ne couvre pas cette erreur.
Les anciennes observations WHO/XYZ sur des montages à fils restent leurs faits
historiques ; leurs descriptions de numéros de broche ne qualifient pas ce PCB.

## Correction logique minimale proposée

Conserver PC9/10/11/12 et les interfaces firmware/V1, l'alimentation sur1/2,
la masse13, l'implantation et les autres interfaces. Réaffecter uniquement :
**CS→19, SCK→20, MISO→22, MOSI→21**, puis refaire ERC/DRC et exports d'une variante
corrective séparée. Le candidat V2.3 fabriqué doit rester archivé sans écrasement.

Pour les prototypes existants, une topologie théorique possible est :

1. Isoler les anciens raccordements des nets ACC_CS et ACC_SCK aux contacts21/22.
2. Raccorder les sources ACC_CS à19 et ACC_SCK à20.
3. Relier le net MOSI disponible sur23 à21, et le net MISO disponible sur24 à22.

Cela représente **deux liaisons à ouvrir et quatre liaisons isolées à ajouter**
au niveau logique. La disponibilité des points, les couches, les lieux exacts de
coupure et l'absence de cuivre voisin à endommager ne sont pas encore qualifiés.
**Ce n'est pas un mode opératoire de découpe.** Ne pas ajouter les ponts avant
isolement des anciens nets : cela créerait des liaisons entre signaux conduits
par des sorties distinctes. Ne pas décaler le module dans son support : ses
alimentations et sa masse doivent conserver leurs positions.

Le choix entre reprise des prototypes et nouvelle fabrication est une décision
humaine. Un interposeur dédié ou un montage externe pourrait aussi séparer les
liaisons erronées, mais sa mécanique n'est pas préparée et il ne qualifierait pas
le shield non corrigé. Aucune dépense ou intervention réalisée.

## Vérifications avant toute future qualification

Après décision et plan détaillé : intervention utilisateur hors tension, contrôle
d'isolement des anciens nets et continuités des quatre nets corrigés, absence de
court-circuit et rails corrects, puis WHO unique7B attendu avant configuration/XYZ.
Le défaut de masse002 doit être corrigé et contrôlé séparément. Le défaut FRAM001
à1 MHz reste indépendant et non résolu par ce constat SPI3.

Le contrat courant est un bring-up avec garde-fou « aucune refonte ». AGENTS.md §8
impose une escalade quand les faits contredisent la spécification ou qu'une
modification hors périmètre devient nécessaire. La qualification alimentée est
suspendue ; proposition à revoir avant extension corrective du périmètre.
