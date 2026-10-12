# M015-E — Décision sur la voie d’acquisition

**Retenir la lecture directe A pour un démonstrateur RAM borné. Viser FIFO en bursts de128 mots pour la V1, après qualification des lectures ; ajouter DMA seulement si les mesures le justifient. Prochaine tranche : comparer fil et RAM pour les commandes88/87/BA à625kHz, si un instrument adapté est identifié.**

Cette décision ne résout pas le défaut SPI et ne modifie pas la V1. M015 reste IN PROGRESS ; B/C/D restent BLOCKED. Aucun changement matériel ou firmware enE.

## Diagnostic : faits et hypothèses

A a fourni2048XYZ cohérents à≈6538 lectures/s, sans prouver toutes les conversions à26,667kHz. B a rejeté sa capture :171statuts,11reculs, zéroXYZ accepté. C a obtenu512 lectures cohérentes au repos. D a montré26/28 divergences CTRL2 single/burst pendant remplissage, puis0/32 après coupure de l’entrée ; WHO etCTRL3 sont stables. Les28 statuts groupés D sont monotones jusqu’à256, avec une cadence différente deB.

E reconstruit les37440octets du préfixe RAM D depuis leCSV et les compare exactement au dump conservé. CTRL2 n’est écrit qu’aux indices15 (`08 81`) et453 (`08 00`, restauration). Aucun write applicatif ne modifie donc CTRL2 pendant les comparaisons. Cela ne prouve pas la vraie valeur sur le fil ou dans le capteur. Preuve : [analyse](Echanges_Codex/preuves/TR2-20261012-M015-E/diagnostic-analysis.json).

| Hypothèse compatible | Discriminant minimal | Limite |
|---|---|---|
| Commande, CS ou signal électrique incorrect | DécodageCS/SCK/MOSI/MISO des lectures88/87 | Une trace logique ne certifie pas les marges analogiques |
| RéceptionMCU/HAL/tampon incorrect | MISO=81 maisRAM=80/02/00 dans la même transaction | Localise après le fil observé ; ne désigne pas HAL seul |
| Readback interne dépendant de la FIFO alimentée | Commande correcte, anomalie déjà surMISO, disparition après coupure | Exige d’abord un signal et des timings valides ; pas de capteur déclaré défaillant |
| Cadence/interleaving/BDU du fastpollB | ComparaisonBA contextualisée et datée | Un défautCTRL2 ne démontre pas la cause des11reculsB |

Le mouvement du voilier n’explique pas une valeur de configuration différente. Une capture sans anomalie serait inconclusive, pas une raison de répéter automatiquement les campagnes.

## Contraintes de débit et de SPI3

ODRnominal26667XYZ/s :37,4995µs/sample,≈6000cyclesCPU à160MHz. XYZint16 :160002octets/s ;FIFOtag+XYZ :186669octets/s. FIFO512slots :19,20ms de remplissage ;watermark128 avec profondeur512 laisse14,40ms de marge idéale, avant overhead et ordonnancement.

Le driver réel lit STATUS puisXYZ :9octets/XYZ, minimum1,920024Mbit/s. FIFO par mot avec unstatut coûte11octets/XYZ, minimum2,346696Mbit/s. FIFO128 :897octets de transfert,900 avec unstatut ;minimum **1,500019Mbit/s**.

| SPI candidat | Occupationfil direct àODR | Occupationfil FIFO128+statut | Tempsfil parbloc128 |
|---|---:|---:|---:|
|625kHz, acquis diagnostic|307,2%|240,0%|11,52ms|
|2,5MHz, candidat mode3|76,8%|60,0%|2,88ms|
|5MHz, candidat|38,4%|30,0%|1,44ms|
|10MHz, plafond capteur|19,2%|15,0%|0,72ms|

Calculs hors overheadGPIO/HAL/DMA et traitement, pas benchmarks. DMA ne rend pas625kHz suffisant. À2,5MHz, lefil laisse1,92ms par période debloc4,80ms ; disponibilitéCPU et latences doivent être mesurées. La cadenceA irrégulière ne permet pas de revendiquer FFT, bande6kHz ou décimation sansaliasing.

[DS12569 §2.4](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) borne leSPI capteur à10MHz. Avec le noyau160MHz enregistré enD, div64/32/16 donnent arithmétiquement2,5/5/10MHz ; aucune de ces vitesses n’est qualifiée enmode3 sur le montage. GPIO_SPEED_FREQ_LOW, fronts, câbles, charge et timing restent à contrôler.

[DS13737 §3.44](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf) distingue **SPI3 limité :FIFO8octets, transfert jusqu’à1024**. Choisir128mots/897octets plutôt que256/1793 ou512/3585 dans une seule transaction. La tailleuint16 duHAL ne supprime pas la limite matérielle. [AN5444 §6.4](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) décrit les multiples de7octets et lewrap0x7E→0x78 ; aucun burstDATA_OUT n’est encore physiquement qualifié ici.

## Comparaison des voies

| Voie | Usage et acquis | Dépendances / risque | Critère d’arrêt |
|---|---|---|---|
| DirectA625kHz | Démonstrateur RAM, données déjà disponibles | Pertes noncomptées, timestamps delecture | Pas revendication de fluxV1/READY |
| Direct rapide, DRDY/DMA | Alternative théorique | Deuxtransactions/sample, deadline37,5µs ; INT1/2 noncâblées ; absence d’indiceconversion | Refuser la continuité sur la seule cadence moyenne |
| FIFO128 en bursts, diagnostic RAM | Première qualification de données proposée après fiabilité des lectures | DATA_OUT/tags/parité/niveaux/overflow/timing à prouver | Rejeter tout lot incohérent ; pas masque du défautCTRL2 |
| FIFO128 +GPDMA/service borné | Cible V1 proposée après qualification | Ownershipbuffers/CS/EOT/RX-TX/ISR, latences maximales et stockage | Pas flux continu avant preuves de pertes et de disponibilité |

Le mode arrêté D/STOP_ON_WTM ne prouve pas un flux continu : celui-ci exige un profil distinct (profondeur512/STOP désactivé à spécifier), pas une modification silencieuse. Le SDK définit SPI3_RX/TX pour GPDMA (requêtes10/11), mais le driver reste bloquant et les IRQ/DMA correspondantes ne sont pas raccordées. DMA peut notifier la fin sans brochecapteur ; DRDY/watermark physique demanderait une route et un branchement qualifiés. Un serviceFIFO pollé reste possible si sa latence maximale est prouvée.

DeuxTX et deuxRX de897octets :3588octets. Fenêtre32768XYZ :196608octets enint16,393216 en3int32,524288 si VibrationSample occupe16octets sousABI à vérifier. Linker768KiB ;ELFM014 conservé :bss62912/data24, minimaheap+stack2560, pas preuve de pile maximale. Les accumulateursRMS/peak peuvent traiter enflux sans conserver toute la fenêtre ; budgetCPU et mémoireDMA owned restent à mesurer.

DWT date lestransactions, pas lesconversions. VibrationSample n’inclut niindiceconversion ni timestamp individuel. Les tags modulo4 ne détectent pas les pertes multiples de4 ; niveaux, flags, tags et délais doivent être croisés. Pas de chronologieUTC ou d’ODR mesurée inventée.

## Frontière V1 et prochaine tranche

[Bloc4 §8](../01_Specification_source/bloc4.md) etconfiguration_validator imposent26667Hz, fenêtres4096/8192/16384/32768, modecampagne1, stockage store-and-forward1 et capacité utilisable. M014 conserve configurationUNAVAILABLE/READY0/stockage0. Un démonstrateurRAM ne satisfait ni la continuité ni le stockage V1. Provisioning/SD et traitements opérationnels versM016 restent des lots distincts avec autorités et sauvegardes explicites.

1. Identifier seulement l’instrument disponible ; aucune demande de câblage enE. Sous contrat distinct, une mesure625kHz suivant [la fiche](MESURE_LOGIQUE_M015_E.md) compare88/87/BA pendant alimentation puisétatfigé, sans rejouer intégralement A-D.
2. Correctif minimal uniquement après défaut démontré ; sinon conserverBLOCKED. En attente d’instrument, préparer contrats/traitements horsligne sur donnéesA existantes, sans nouveau matériel ni qualification présumée.
3. Après lectures fiables, qualifier un seul profilRAM FIFO128 à une fréquence choisie souspreflight. DMA/serviceV1 et stockage viennent ensuite ; utiliser lesbursts pour avancer ne vaut pas réparer ou masquer CTRL2.

Preuves chiffrées : [calculs](Echanges_Codex/preuves/TR2-20261012-M015-E/bandwidth-memory.json). Ce texte est une décision de développement, pas un GOmatériel.
