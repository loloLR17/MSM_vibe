# Ce qui a changé depuis le dernier fonctionnement — analyse hors ligne

Analyse sur HEAD `97fa3d0ae242eb8db7b3dc2bfa8c9005f04d548f`. Aucune intervention matérielle, aucun build, flash, reset, accès Windows/COM/SD ou nouvel essai. Les validations réussies sont réutilisées. Commande exécutée :

```bash
python3 /home/lolo/dev/msm/reports/TR2-r3-last-working-review-20261031/compare_last_working.py
```

Le script joint lit les images, options et diagnostics déjà conservés ; il ne commande aucun appareil. Code de retour 0, assertions satisfaites. Les BIN N8A/B/C/D locaux sont aussi comparés aux BIN déjà publiés dans Git à97fa3d0a : identiques. Les octets bruts sont comparés avant toute normalisation ; seuls les deux champs ASCII explicitement identifiés sont ensuite normalisés pour le contre-contrôle SHA. Aucun décalage de données de mesure n'est corrigé.

## Deux références à distinguer

- **Dernier succès long : E6N**, sourcec3cd40ce, BINb70438c7…925330, 6h exploratoires récupérées intégralement avec0rails/clips ; réserve historique de digestRAM indépendant perdu maintenue. Le lieu «plancher du PC machine» et l'installation proviennent de l'utilisateur/du contrat, pas d'une observation physique Codex.
- **Dernier succès plus récent : N8A**, source d5af0828, BINc31904fe…3ddc17 : calibration puis gate2s réussis le31/10 avant déplacement vers la machine. Ce succès a déjà le profil8h/30600sMCU/FAT640MiB/SPI3=8MHz et le même capteur initial selon l'échange utilisateur. Il est donc plus discriminant que de comparer directement E6N au dernier essai duplex.

## Chronologie établie

Heures ci-dessous = **annotations UTC des journaux hôte** ; ni UTC des échantillons ni mesure de durée SI déduite.

| Moment | Conditions ou changement | Résultat réellement observé |
|---|---|---|
| Campagne6h antérieure | E6N, ancien capteur ; plancherPCmachine selon utilisateur | Fichier récupéré COMPLETE/0rails/clips, qualification indépendante limitée après coupure |
| 18:19–18:22 | Nouvelle imageN8A de préparation, avant déplacement ; SPI split8MHz | Calibration16µs, gate2,000357s/5294sorties réussi puis relecture intégrale FatFs/SHAamont conforme |
| Entre préparation et retour | Déplacement du T1000/TR2 vers machine rapporté par utilisateur ; maintien d'alimentation espéré | Instant et mécanisme de redémarrage non enregistrés |
| 21:16 | N8A toujours en Flash, entière relue identique | RAMWAITinitial2/run0/compteurs0/digestnul : redémarrage observé, avant tout flashN8B |
| 21:30 | RéarmementN8B pour nouveaux noms ; code exécutable inchangé | **Calibration réussie40µs en conditions post-déplacement**, puis gate arrêté0,205253s sur paritétag, avant premierSDgroupACK |
| 21:32 | N8C ; seul préfixeB→C différent par rapport àB | Calibration refusée sur taginconnu ; aucun gateSD |
| Après les premiers échecs | Réenfichage puis mesures3,3V ; ensuite duplex, essais de fronts/fréquence | Changements et essais postérieurs au premier défaut, tous refusés ; pas des changements qui expliqueraient chronologiquement son apparition |
| Comparatif utilisateur ultérieur | Capteur deTR2-001 emprunté, imageDduplex8MHz inchangée ; un resetMCU nécessaire | Même refus de calibration temporelle,1875862µs>100 ; pas de preuve de défaut propre au capteur initial |

Autre distinction utile : au premier défautB, les compteurs de captureSD donnent `sd_steps=0`, `sd_written=0`, `sd_completed=0`, `sd_pending_polls=0`, file3072B. Le code ne démarre un groupeSD qu'à4096B : aucun groupe d'écriture de données n'avait commencé pendant ce court enregistrement. La préparationFAT/header/footer avait eu lieu avant le FIFO ; cela n'exclut pas un effet antérieur ou électrique de la carte, mais ne démontre pas une panne causée par la coexistence d'un transfertSD actif. La calibration et le gate ont par ailleurs des charges/timings différents (traitementFIR, témoin, file), avec le même code dansAetB.

La campagne6h avait déjà réussi au lieu demandé selon le contrat. **La présence en machine, à elle seule, n'est donc pas une cause démontrée.** Le cheminement exact des liaisons, leur contrainte mécanique, les masses, l'alimentation et le régime des équipements environnants n'ont pas été enregistrés de manière comparative. Leur changement reste une hypothèse, pas un fait établi.

## Comparaison logicielle plus forte qu'un simple diff source

| Comparaison | Preuve |
|---|---|
| E6N→N8A | Sources d'exécution et CMake de la chaîne inchangés dans Git ; options durée22e9→30,6e9µs et préfixe/provenance différents. Ce ne sont pas des BIN identiques et le succèsN8A démontre déjà le nouveau profil avant déplacement. |
| N8A→N8B | **54556octets chacun ;36octets différents, tous dans le SHAASCII de provenance et le nomN8A→N8B. Tous les autres octets sont identiques**, options de compilation identiques hors ces champs. Aucun changement de programme exécutable, de bibliothèque ou de placement des instructions dans ces images. |
| N8B→N8C | **Un seul octet différent**, le caractèreB→C du préfixe à l'offset53473. Aucun autre octet changé. |
| N8C→N8D | Changement réel de transport introduit **après** les échecs : voie existante duplex sélectionnée, source98457b5a. Les essaisD ne doivent pas être confondus avec la référenceN8A split. |

SHA commun après normalisation des seuls champs provenance/préfixe : `a86b38f65e953e943ac8f66bf82c94452ba2e21aa75c12fc48b1c4b28bb2f391`. Offsets, champs autorisés, différences brutes, hashes et paramètres dans COMPARISON.json. Cela exclut une **modification du code exécutable entre A et le premier échecB** ; cela n'exclut pas un défaut préexistant sensible aux états, aux données ou au timing.

## Registres réellement sauvegardés : identiques avant/après

Les snapshots de Acal/Agate/Bcal/Bgate/Ccal donnent tous :

- SPI3 CFG1=`0x00070007`, CFG2=`0x87440000` ; même configuration sauvegardée du transport split8MHz.
- SPI2 CFG1=`0x10070007`, CFG2=`0x84400000` ; même configuration sauvegardée SD4MHz.
- UID002, CID`0353445344333247858bf0923c01927b`, clockdéclarée160MHz identiques ; PRIMASK/BASEPRI/FAULTMASK=0.
- Même FIR107/Q15/décimation10, gain, FSlogique8g, formatv3, contrôles temporels/qualité : les images A/B/C, hors leurs chaînes identifiées, sont identiques.

Ce sont des configurations/états observés ponctuellement. Ce n'est pas une mesure électrique de SCK, ni une preuve que tous les états initiaux du capteur étaient identiques. La lecture du chemin `main→calibrate→fifo_live_start` montre un arrêt/configuration du capteur, **sans reset global explicite de l'IIS3DWB dans cette séquence**. Un reset/reflash du MCU n'établit donc pas à lui seul que l'état global du capteur a été réinitialisé de la même manière qu'une coupure physique. Aucune différence d'état capteur non observée n'est présentée comme cause.

## Ce qui reste inconnu et conclusions proportionnées

1. Les indicateurs RCC de cause du **premier** reset n'ont pas été sauvegardés à21:16 avant les reflashes/resets ultérieurs. Sa cause exacte ne peut donc pas être réattribuée rétrospectivement avec ces preuves. Pas d'affirmation certaine de coupure, brownout ouNRST.
2. L'image **N8Aexacte n'a pas été réessayée après le déplacement** : les comparaisons réelles ont commencé avecB puisC. Leur code est prouvé identique hors champs, mais un contrôle de référence exact reste manquant.
3. La calibrationB réussie puis le gateB rejeté, et le refusC avec un seul octet de nom différent, suggèrent un comportement variable ou dépendant des états/conditions. Ce n'est pas la preuve d'un câblage faux permanent ni d'une panne permanente du premier capteur.
4. Les symptômesB/C(parité/tag), D(timestampsdirects) et D4MHz(configuration) sont distincts. Une origine unique n'est pas démontrée. Les modificationsduplex/fronts/fréquence et le capteur de référence sont des essais ultérieurs, pas des causes antérieures au premier défaut.
5. Aucun de ces constats ne qualifie l'acquisition8h. La préparation reste refusée avec les gardes conservées. Le câblage n'est pas condamné par défaut et un problème latent de transport/initialisation n'est pas exclu.

## Prochaine vérification la plus discriminante — proposée, non exécutée

Revenir à **l'image N8Aexacte sauvegardée** et au montage/capteur de référence identifié, après consignation des conditions physiques et des états initiaux pertinents, puis ne tenter que l'initialisation/calibration en lecture. Cette commande ne crée pas de fichier : aucun besoin de supprimer ou d'écraser le gateN8A0003 existant. Toute réutilisation matérielle doit conserver les préflights et autorisations de mission, sans forcer les étatsRAM ni assouplir100µs.

Avant toute autre variante, documenter ce qui est effectivement identique/différent : module capteur actuellement emprunté versus original, liaison/cheminement/fixation, alimentation/chargeur/ports et état initialMCU/capteur. Si l'on compare ensuite l'état capteur réinitialisé, le faire comme **un seul facteur explicitement maîtrisé**, après sauvegarde des états utiles ; pas de nouvelle campagne exploratoire multidimensionnelle. Aucun de ces essais n'a été effectué dans cette étude demandée par l'utilisateur.
