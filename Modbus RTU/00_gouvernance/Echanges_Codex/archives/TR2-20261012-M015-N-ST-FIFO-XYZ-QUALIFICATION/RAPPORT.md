---
mission_id: TR2-20261012-M015-N-ST-FIFO-XYZ-QUALIFICATION
status: BLOCKED
base_ref: main
base_sha: 83f9cb80bc292bd24dda878a8e99b85085f1d664
initial_head: ddd292fb06aed40e06fe39dfc7ab6230344cdabe
result_sha: 64f55db78dfb5cfc1392939fd688691a6977b578
created_at_utc: 2026-10-12T21:22:09.324310+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-N — harness corrigé, qualification FIFO XYZ bloquée

**La référence atteint maintenant 512 entrées avec un cleanup conforme. Aucun
XYZ n'est obtenu dans la référence ni dans les trois expériences diagnostiques
autorisées.** La correction du journal et du cleanup est acquise ; la cause
précise de l'absence de batching XYZ reste inconnue. Statut **BLOCKED** technique.
Les conditions nécessaires aux extensions 512 XYZ et 4096 XYZ ne sont pas vertes :
ces extensions n'ont pas été engagées. Aucun quatrième diagnostic, aucun replay.

## Référence Git, autorisations et préservation

HEAD initial : `ddd292f`, main réellement récupéré ; base du contrat :
`83f9cb80bc292bd24dda878a8e99b85085f1d664`. Worktree isolé `codex/m015-n`,
permissions commit/push vers main, flash/debug exercées dans le contrat M015-N.
AGENTS.md, protocole V2 et journal consultés. Les mission/rapport M015-M sont
archivés exactement depuis 83f9cb8 ; leurs preuves restent intactes.

Le dépôt principal main historique `82169af` et les worktrees précédents restent
propres, avec leurs HEAD inchangés. Les deux stashes sont conservés et vérifiés.
Aucun accès au TR2 principal, à M014-H, FRAM, microSD, HP, COM7 ou Modbus.
Aucune modification de spécification gelée ni de source de production TR2.

## Audit causal avant matériel

Les 1024 transactions M015-M ont été reconstruites depuis les traces et payloads.
Les deux lectures de 896 octets reproduisent exactement les 256 mots RAM.
Un analyseur indépendant confirme les tags timestamp, la parité et le découpage
par sept octets ; aucune erreur d'offset, d'endianness ou de classification
n'explique les 256 timestamps et zéro XYZ. Une commande reconstruite depuis
registre/sens constitue une lecture des traces, pas une capture wire.

[Audit, matrice d'hypothèses et budgets](../../../experiments/iis3dwb_st_reference/AUDIT_M015_N.md).
Le [datasheet ST DS12569 Rev 8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf)
a été consulté : les champs BDR=0A, XL_EN=5, stream=6 et décimation timestamp=8
correspondent aux réglages employés. Les deux getters de la version ST pinnée
restent défectueux : masque de niveau 03F, et case 10 au lieu de 0A pour BDR.
Le commentaire exemple «12.5Hz» diffère de son argument 26k7 effectivement utilisé.
Aucun patch vendor : sources ST et SDK original vérifiés byte-identiques.
AN5444 était inaccessible par timeout ; aucun contenu de cette note n'est cité.

La matrice prévoyait une référence puis trois diagnostics : désactivation du
batching timestamp, programmation tardive du BDR, lecture par mots de sept octets.
Chaque diagnostic change un paramètre causal annoncé ; aucun changement de
CPU/SPI/FS/ODR/BDR/mode simultané. Les variantes sont clairement distinguées
de la référence ST. Le profil word7 limite aussi l'observation à huit entrées,
sans changer la configuration du capteur ; il ne qualifie pas 512 lectures courtes.

## Correction et validation logicielle

Le journal réserve **12 traces, 24 snapshots et 32 octets** au cleanup
(besoin : 9 transactions, 18 snapshots et 9 octets). Acquisition limitée à
1012 traces/2024 snapshots/65504 octets. Les refus de trace, payload et snapshot
sont distincts du débordement de l'observateur ; aucune preuve n'est effacée.
La garantie porte sur la saturation du journal, pas sur une panne matérielle HAL.

Les polls sont bornés à 400 et 500 ms, espacés de 1 ms. Un agrégat compact conserve
nombre, chronologie, niveaux min/max et flags cumulés, en plus des bruts.
Le calcul majorant donne 901 traces pour la référence et 953 pour word7/8,
moins de 6 KiB de payload et au plus 1906 snapshots. Les buffers restent à
1024 traces, 64 KiB et 2048 snapshots ; aucune augmentation arbitraire.

Le cleanup conserve la première erreur, appelle les setters ST XL off/bypass/
timestamp off puis exige trois readbacks. HAL SPI seul reste optimisé O2,
CPU 16 MHz, SPI mode 0/2 MHz calculés et timeouts de 20 ms inchangés.
L'observateur avant CloseTransfer et les critères de réception complète sont maintenus.

Validations réellement exécutées :

- 5/5 CTest : 14 scénarios diagnostic plus 8 cas de budgets ; intégration initiale
  41 cas puis 43 après ajout des refus de capacité et d'échec de cleanup ;
  14 tests Python d'analyse indépendante, 2 d'instrumentation et 5 d'outillage.
- Positifs/négatifs : tags mixtes ou timestamp seul, parité, SPI, wrap/gap compteur,
  overflow FIFO, wrap/recul temporel, ratio timestamp/XYZ, fenêtre partielle,
  troncature et cleanup impossible. WHO/polling passent les mocks de régression.
- Quatre cross-builds ARM et GCC `-fanalyzer` ciblé réussis. Avertissements Newlib
  usuels conservés. Aucun test/build de production TR2 ou .NET inchangé rejoué.

Cette validation est proportionnée au projet autonome modifié. Les tests hôte
ne valent pas preuve physique. Les anciens WHO/polling physiques n'ont pas été répétés.
Checkpoint logiciel `64f55db78dfb5cfc1392939fd688691a6977b578`, message
«M015-N: reserve cleanup budgets and qualify FIFO independently before hardware» :
push normal et relecture intégrale de 38 fichiers identiques **avant** les essais.
Les quatre images physiques proviennent exactement de ce code.

## Campagne physique consommée

Seconde NUCLEO-U575ZI-Q uniquement : SN `0032001F3235510C37333439`,
UID `001B00463842501820333157`, MCU482/RevU, tension CLI 3,28 V.
Identité relue avant chaque flash ; SHA BIN/ELF vérifiés ; NORMAL/SWD/SN explicite,
adresse 08000000, verify et readback byte-identique. Après le dernier CLI,
reset GDB puis GDB seul. Boot ready=1/core16MHz/UID exact/faults=0 ;
un trigger par image, detach pendant exécution. Tous GDB/serveurs se terminent à 0.
Les contrôles WHO internes appartiennent à l'initialisation ST, pas à un essai WHO autonome.
Le câblage et les tensions VDD/VDDIO restent des confirmations utilisateur antérieures.

| Profil unique | Paramètre | Observation | Traces / polls | Cleanup |
|---|---|---|---:|---|
| Référence ST/STATUS10 | Configuration ST inchangée | 512 timestamps, 0 XYZ ; result0 | 285/93 | 0 ; 0C/80/00 |
| no-ts | DEC_TS_BATCH 2→0 ; TSON reste 1 | FIFO vide pendant 500 ms ; result4 | 543/250 | 0 ; 0C/00/00 |
| bdr-last | BDR programmé après XL/TSon ; mêmes valeurs finales | 512 timestamps, 0 XYZ ; result0 | 285/93 | 0 ; 0C/80/00 |
| word7 | Burst128→1 ; borne d'observation8 | 8 timestamps, 0 XYZ ; result0 | 257/51 | 0 ; 0C/80/00 |

Toutes les transactions enregistrées sont valides, sans erreur HAL/SPI. Tous
les snapshots RX ont restant=0, offset=taille et absence d'OVR avant nettoyage.
Aucun refus de journal ni overflow. Le timeout no-ts est celui de l'attente de
fenêtre de 500 ms, **pas un HAL_SPI_TIMEOUT** ; cette issue prévue a été conservée
et analysée avant l'expérience suivante. La référence et bdr-last ont chacune
quatre lectures896 complètes ; word7 a huit lectures7 complètes ; no-ts aucun drain.

Readbacks requis avant et après chaque burst conformes : 07..0A/CTRL1 =
00/01/0A/86/AC, sauf no-ts = 00/01/0A/06/AC ; CTRL10=20 avant cleanup.
Getter BDR=0 conservé comme télémétrie, jamais substitué au registre brut 0A.

## Intégrité et limites

Les `count×7` octets renseignés de RAM sont byte-identiques aux payloads capturés.
Référence et bdr-last : 512 tags21h/type4, parité correcte, compteur0,
timestamps751→13015 avec 511 pas de24 ticks. Word7 : huit tags21h/type4,
timestamps751→919 avec sept pas24 ticks. Les zéros BSS ne sont pas des XYZ.
L'interprétation nominale de 12,5 µs/tick n'est pas une mesure indépendante de l'ODR.

Niveaux observés : référence0..260, bdr-last0..261, word7 0..344, no-ts toujours0.
Aucun FULL/OVR/latch dans les statuts lus ; les lectures discrètes ne prouvent
pas une absence globale de pertes. Aucun nouveau statut après le dernier burst
avant cleanup : bypass est confirmé par readback, pas par une mesure ultérieure du niveau.
Lectures896 : 3,6324..3,6328 ms ; lectures7 : 48,125..48,375 µs, observées via DWT.

L'analyseur rejette toutes les fenêtres pour `no_XYZ`. Zéro discontinuité XYZ
avec zéro XYZ est un compte vide, pas une preuve de continuité. Les compteurs
constants timestamp ne démontrent pas zéro perte : `global_loss_zero_proven=false`.
Aucune fenêtre XYZ acceptée, aucune qualification4096/32768/V1/FFT/production.

## Diagnostic et prochaine action

Le défaut de capacité/cleanup M015-M est corrigé et le transport long reçoit
les tailles complètes. Le parsing n'explique pas les timestamps ; supprimer
leur batching laisse le FIFO vide. Déplacer le setter BDR ne produit pas de XYZ ;
la lecture courte n'en montre aucun dans les huit premières entrées.

La piste restante prioritaire est le batching XL effectif ou l'état capteur/
chaîne d'écriture, malgré BDR relu0A. **Aucune cause silicon, HAL, câblage ou
module défectueux n'est attribuée sans preuve.** Les getters erronés ne suffisent
pas à expliquer les payloads décodés indépendamment.

Les trois diagnostics autorisés sont consommés. Préparer un nouveau contrat
pour une observation indépendante SCK/CS/MOSI/MISO des écritures09/10/0A/19 et
lecturesFIFO avec un instrument3,3V identifié, ou un autre témoin contrôlé à un
seul facteur. Vérifier la référence/révision matérielle et justifier un éventuel
powercycle si cette hypothèse est retenue ; les gestes physiques restent à
l'utilisateur. Aucun geste supplémentaire demandé ici. La comparaison vers
le TR2 principal reste différée jusqu'à une voie FIFO XYZ qualifiée sur ce banc.

## État final et publication

La seconde NUCLEO reste sur word7, inactive après trigger consommé : XL off,
FIFO bypass, timestamp off, CFSR/HFSR0, debugger détaché. Aucun backup Flash2MiB
ni restauration initiale répétés, conformément à la règle du banc jetable.
Aucun mass erase, Option Bytes, RDP ou changement de boot persistant.
Journal et roadmap actualisés : résultats, blocage et prochaine preuve.

[Preuves et index](preuves/TR2-20261012-M015-N/README.md), originaux
`/home/lolo/dev/msm/reports/TR2-mission015-N-20261012T210706Z` : commandes,
identités, hashes, images/readbacks hex, logs GDB, result4040/Trace40/Preclose76
little-endian, bruts, CSV, analyseurs et manifestes. Petites erreurs d'outillage
corrigées : curl absent, timeout documentaire, quotage du helper Python avant
connexion MCU ; aucune acquisition supplémentaire ni erreur test/build cachée.

| Image | Taille BIN | SHA256 BIN |
|---|---:|---|
| reference | 31392 | `df6e49076ea57d22e8c305e11fa7645900a1153fdecbf47d4f28df80003911ce` |
| no-ts | 31392 | `62ef50677fb049461784c25d30a48e3d3514c525e9f2237e132a1a9ab9ddabb5` |
| bdr-last | 31392 | `425090e8299102aa9b4c64b4e122a2f28eb463fa2191e04a0833c8b8b6d1a4fa` |
| word7 | 31392 | `8ece594a3102d2c79be27fe3c06e5a20daf67ecce252fd613dc3c9e25d48a545` |

Résultats publiés au `5f965411b5628f60669a529d2100b41df1b15598`, message
«M015-N: publish bounded FIFO diagnosis and safe cleanup evidence» ; push normal
vers `HEAD:refs/heads/main`, fetch puis **162 fichiers intégralement relus,
byte-identiques**. La présente mise à jour consigne cette preuve distante et
améliore la lisibilité du rapport ; son SHA final sera fourni après vérification.
Worktree N propre après publication, original/main historique conservé.
Aucun merge/rebase/reset/force. BLOCKED reste technique, pas attente de validation
intermédiaire utilisateur ; la publication n'est pas une qualification FIFO XYZ.
