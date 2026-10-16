# R4-D — découpage causal START / observation

Mission `TR2-20261016-R4D-START-TIMING-DIAGNOSTIC`. Diagnostic isolé, aucune
admission R2 ni écriture FRAM. Le verrou STARTED epoch2/run2 reste inchangé.
Ce document ne change ni R0, ni V1, ni les limites1000us/2500us/gap5ms.

Le probe opt-in R4D utilise la recette matérielle cumulative, le vrai port
`LiveFifoAcquisition` et le même transport ST/SPI3 4MHz. Il attend un trigger RAM.
`R4_BENCH=ON,R4D_DIAGNOSTIC=ON,R4M_MAINTENANCE=OFF` produit une image distincte
NON-PRODUCTION. Aucun UART/SD ; symboles submit, isolation et write FRAM absents
de l'ELF. UID et SHA intégral FRAM f2bd7a99…8a0c1 sont vérifiés avant exercice.
Les identifiants de diagnostic0x5234440000000001+n sont volatils, non admis R2.

## Défaut observé avant correction

Sur TR2-001, deux séquences sans wrapper : START905,64/906,55us,
OBSERVE102,34us, intervalle combiné1008,26/1009,18us ; STOP235,64/235,47us.
Exercice sous garde1000us : déclenchement1012us, arrêt sûr, aucune écriture.
Il s'agit d'une reproduction du défaut de découpage ; les1241us historiques
R4-M incluaient d'autres opérations CPU. Aucune pile au point exact historique
ne permet d'attribuer chaque microseconde rétroactivement.

Trace proportionnée :36callbacks SPI dans START (dont9 de STOP préalable),
3dans OBSERVE,9dans STOP. Aucun résultat SPI défaillant. Calibration128appels
vides :63cycles/appel direct,217wrapper sans append,293avec append ; surcoût
230cycles=1,4375us à160MHz. Les temps globaux non instrumentés sont la référence ;
le surplus tracé≈58us sur START+OBSERVE concorde. Pas de calibration externe/WCET.

## Correction minimale et causalité

Le moteur R2 dispose de trois points de service explicites : avant effet,
entre effet et observation indépendante, puis après observation avant encodage
et persistance du reçu. Aucune vérification capteur/CRC supprimée, aucune phase
ou limite relâchée. Sans contexte coopératif, les checkpoints sont des no-op.

Une erreur de service entre effet et observation ou entre observation et reçu
conserve STARTED/INTENT et déclenche l'arrêt indépendant. Elle ne prouve jamais
l'absence d'effet ni un succès durable. Le STOP de sécurité ne dépend pas d'un
checkpoint/NVM ; sa routine essaie toujours toutes les actions et leurs relectures.
Un callback unique trop long reste refusé : cette correction ne donne pas un
WCET de START ou d'un bus en panne.

## Validation et suite préparée

Le nouveau test138 échoue avant correction, passe après ; scénario indépendant
905us START+103us OBSERVE, mêmes262144octets finaux que START/STOP non coopératifs,
fautes aux deux nouvelles barrières sans write/replay, effet1100us toujours refusé.
Suite complète138tests PASS56,80s, y compris1561oracles de coupure/recovery existants.
Cross-build canonique/probe PASS, BIN V1 inchangé6a2150af…15a9.

Exercices physiques après correction préparés avec trigger0x52344402 :
quanta explicites autour du vrai port (sans moteur R2/admission), puis erreur
logicielle unique avant le11e callback de START et arrêt/relecture réels.
Ce n'est pas une panne électrique SPI. Les anciens exercices réussis ne sont
pas relancés par ce trigger. FRAM complète à comparer après chaque image,
puis Flash initiale à restaurer et vérifier intégralement.

## Limites HAL et périmètre de preuve

Chaque callback SPI3 a deux appels HAL polling, commande puis payload, timeout20ms
chacun, sans retry ; CS relâché et statuts/compteurs/préclose contrôlés. Le timeout
emploie HAL_GetTick ; il ne borne pas l'exécution si le tick ne progresse pas.
Une faute peut dépasser1ms avant détection ; le STOP sûr utilise ce même bus,
sans garantie électrique de succès. Les mesures nominales ne qualifient pas
IRQ masquées, bus bloqué, panne électrique, FIFO sous charge, RTU2 ou R6.
La campagne R2 physique et la réconciliation du verrou nécessitent une mission
ultérieure distincte ; aucun nettoyage/reprovisionnement autorisé ici.
