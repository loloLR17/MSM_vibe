---
mission_id: TR2-20261031-R5-BV-PK-ALTERNATIF-6H
status: BLOCKED
base_ref: main
base_sha: 95ef8c8bc03b35e0ad9d7ad889ca370070292de3
initial_head: e0887972f12fd12e520e2947d3f7e85716be0265
result_sha: 7ed32c4e5df217bab554f548291e97e12d23afa8
created_at_utc: 2026-10-31T11:18:08.996899+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R5-PK — campagne traitée, comparaison globale indéterminée

Les six heures réelles sont traitées et les livrables candidats sont terminés.
**Aucun grade BV ni jugement favorable n'est attribué.** La comparaison globale
de la paire accélération/vitesse est `METHODOLOGY_INDETERMINATE` : aucune
fenêtre-axe recevable en accélération, 11 en vitesse par hypothèse de cadence,
et aucune minute recevable sur les trois axes. Le statut `BLOCKED` porte sur
l'objectif scientifique de comparaison PK globale, pas sur une attente de
publication : les résultats candidats et leurs preuves sont publiés et relus.

## Résultats exploitables et limites

Le maximum candidat sur XYZ de chaque fenêtre de 60 s donne les statistiques
suivantes sous l'hypothèse MCU. Ces valeurs incluent des composantes rejetées
et **ne sont pas des valeurs de confort qualifiées**.

| Grandeur candidate | Médiane | P95 | Maximum |
|---|---:|---:|---:|
| Accélération 1–5 Hz, mm/s² crête | 4,8 | 7,8 | 11,5 |
| Vitesse 5–100 Hz, mm/s crête | 0,5 | 0,8 | 2,8 |

Les valeurs non arrondies restent dans les CSV et SQLite. L'autre hypothèse
(QPC estimé) donne respectivement les mêmes amplitudes d'accélération et
0,539648935 / 0,835843993 / 2,739231597 mm/s en vitesse. Les deux variantes
utilisent les mêmes fenêtres MCU ; elles ne sont pas deux captures indépendantes.

Sur les 367 fenêtres principales, 365 ont des candidats finis ; la première
contient l'échauffement FIR et la dernière est partielle. Chaque bande/horloge
comprend 1 101 lignes d'axes à 60 s et 6 600 à 10 s : **30 804 lignes au total**.
Les invalidités restent explicites, sans les remplacer par zéro. L'archive
contient 58 552 136 XYZ, 11 sorties d'échauffement et aucun rail/clipping détecté.

Les 11 fenêtres-axes recevables en vitesse (Y : 1, Z : 10), par hypothèse,
ont des crêtes MCU de 0,152323548 à 0,742861111 mm/s. Seules ces composantes
quasi stationnaires peuvent être situées sous le premier repère contextuel,
à titre indicatif. Cela ne qualifie ni leur minute entière ni le local.
Les graphiques n'appliquent pas de barre BV aux candidats rejetés.

L'épisode candidat ACC le plus fort à 60 s commence à 1 800 s MCU :
Z, 11,541088359 mm/s² vers 1,48664749 Hz, **non résolu**. Celui de vitesse
commence à 17 940 s : Z, 2,757813351 mm/s vers 20,780071 Hz, **modulé**.
Le maximum des sous-fenêtres de 10 s atteint 21,63748 mm/s² et 3,09854 mm/s,
sans qualification BV. Les sources mécaniques ne sont pas identifiées.

## Référence et méthode

La [source primaire BV NR467 F, juillet 2026](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf)
a été lue en texte et sur les pages rendues localement : chapitre 6, section 1
§2.1.3(b), section 3 tableaux 5 et 6. `C_F=1` s'applique à une composante
quasi sinusoïdale : crête = √2 × RMS **de cette composante**. Aucune conversion
du RMS large bande précédent. Indices PDF à partir de zéro 238/246/247,
soit pages imprimées 237/245/246 ; voir la fiche de méthode pour éviter
l'ambiguïté de numérotation. SHA du PDF local :
`5ae75d30f6b38a1331431e16bcc3dce11bc42c90b393f344be89b42cb050ceef`.
Aucun PDF complet ni capture de page protégée n'est publié.

| Référence contextuelle : PC machine occupé en permanence à la mer, correspondance à confirmer | 1PK | 2PK | 3PK |
|---|---:|---:|---:|
| Tableau 6, accélération 1–5 Hz, mm/s² crête | 125 | 141 | 157 |
| Tableau 5, vitesse 5–100 Hz, mm/s crête | 4,0 | 4,5 | 5,0 |

Le tableau 4 des RMS pondérés 1–80 Hz décrit une autre grandeur. ISO6954:1984
est retirée mais reste citée par BV2026. Les explications ABS/LR sur les pics
répétitifs ne changent pas le facteur BV et ne rendent pas un peak-hold spectral
équivalent au maximum répétitif d'un signal modulé. Aucune tolérance de points
de locaux n'est appliquée à une proportion de minutes.

L'estimateur utilise Hann périodique, correction du gain cohérent et de la raie
hors bin, interpolation fréquentielle et intégration de la composante pour la
vitesse. Les trois candidats dominants par axe/bande sont conservés dans les
résultats structurés. Le choix principal est confronté à six sous-fenêtres de
10 s : contraste, forme, raies proches, modulation, bords de bande, fréquence
et stabilité d'amplitude ont des gardes explicites. Le maximum court retenu
n'est pas présenté comme une MRA garantie. L'ajustement temporel indépendant
utilise sinus/cosinus, offset et dérive, sans fenêtre Hann.

Tolérances fixées avant essais : sinus synthétique 1 % / 0,002 Hz ; contrôle
réel 5 % d'amplitude / 0,02 Hz, puis 5 % sur le sous-segment de 10 s retenu.
**18 contre-calculs réels à 60 s**, couvrant faibles/médians/forts, et six
recoupements à 10 s ont été conservés. Quatorze contrôles de 60 s concordent ;
quatre échouent, correspondant à deux plages ACC sous chacune des horloges.
Les divergences ACC atteignent 11,50 % et 31,00 % ; ces cas étaient déjà
non recevables. Les composantes vitesse recevables ont des écarts de 0,83–1,25 %
à 60 s et 1,70–2,37 % sur le 10 s retenu. Les six contrôles complémentaires
couvrent les extrêmes globaux de vitesse, sans répéter les premiers segments ;
leur accord n'efface pas leurs statuts modulés/non résolus.

La sensibilité de durée est importante : ratio max des candidats 10 s / candidat
60 s, médiane 2,23 en accélération et 1,77 en vitesse, P95 3,41 et 2,94.
Le candidat dominant peut changer de source ; ces ratios ne sont pas des facteurs
de conversion BV. Les sensibilités aux deux horloges et les contrôles avec
fenêtres temporelles différentes restent publiés.

## Source, métrologie et préservation

Source immuable locale :
`/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2`.
SHA entier avant/après :
`91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5`.
Lecteur strict R3 réutilisé, CRC/index/temps/session/footer vérifiés ;
351 312 816 octets utiles, SHA utile
`5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41`.
La queue préallouée n'est pas traitée comme des XYZ.

Sensibilité nominale 0,244 mg/count, ±8 g, pas d'étalonnage matériel.
Cadences estimées : MCU 2661,462021501383 Hz et QPC 2679,5162204534963 Hz.
UTC inconnue, montage double-face et axes navire non qualifiés, occupation
permanente du local non confirmée. La finesse du bin ne prouve pas une précision SI.
Le digest producteur RAM indépendant est perdu après coupure : R3 reste
`CLOSED_WITH_RESERVATIONS`, seule l'intégrité interne est reconfirmée.

FIR 107 coefficients Q15, SHA
`0e528c6d985207c265ccaa4a5df797ccdfa5ccfe938ad9a51a6a6ad8c94cc1b4`,
identique au firmware archivé. Réponse calculée à 100 Hz MCU : −0,000513 dB,
première zone image échantillonnée à fs_sortie−100 : −77,27 dB. Ce point
ne borne pas à lui seul tout l'aliasing ni le bruit matériel. Retard MCU
≈1,991 ms ; quantification int16 ±0,122 mg, borne numérique d'amplitude
à fréquence fixée ≈0,288 mg après correction Hann. Ce n'est ni une incertitude
physique totale ni une qualification du bruit à 1 Hz. Aucune conclusion
sanitaire, structurelle ou de causalité machine.

## Logiciel, tests et persistance

Code numérique réellement évalué : `7ed32c4e5df217bab554f548291e97e12d23afa8`
(core/run inchangés ensuite). Les ajouts évalués ensuite, publiés dans `23fee5637ff1a9585dd39caacbbf7dfb6a08b7ca`,
portent sur rendu, requêtes, contrôles supplémentaires et enregistrement de
leur audit SQLite ; les 11 tests conservés couvrent cette version.

Commandes effectivement exécutées avec succès, dans le venv R5 existant :

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -m unittest discover -s experiments/r5_bv_pk -p 'test_*.py' -v
```

**11 tests PASS**, dont oracles sinusoïdes sur/hors bin, limites 1/2/4,9/5/
5,1/20/80/99/100/101 Hz, multitons, raies proches, AM/FM, impulsions, bruit,
DC, qualité, clipping/gaps, unités/√2, sérialisation NumPy et migration sûre.
Les anciennes suites/RMS inchangés n'ont pas été relancés. Aucun build STM32
ou .NET ne peut apporter de preuve supplémentaire à cette mission PC isolée.

Le traitement `run.py SOURCE full-components-fixed-v1` a réussi : 100,447 s,
pic RSS 375 256 KiB (≈366,5 MiB), lecture en flux bornée par minute/blocs.
Le premier parcours a échoué à l'export d'un booléen NumPy sans sauvegarder
ses résultats : une reprise PC était nécessaire. Conversion corrigée/testée
et checkpoint d'agrégats sauvegardé avant export/contre-calculs. Les échecs
et leur correction sont conservés ; aucun essai physique n'a été répété.
Les rendus suivants lisent les agrégats et ne répètent pas la FFT complète.

SQLite v3 est une **nouvelle copie migrée** du catalogue v2, avec une seule
campagne, quatre analyses PK, 30 804 résultats, 18 contrôles et 24 références
d'artefacts PK. Les cinq tables historiques et leurs 2 204 RMS sont identiques
sur toutes leurs lignes. Double import idempotent, ajout d'audit idempotent
également en octets, integrity_check=ok, foreign_key_check vide. Les six
requêtes du fichier SQL ont été exécutées (comptes 4/7701/1101/1/0/1).
Les originaux v1/v2, contenant les diagnostics 2–25/PSD et RMS1–80, sont inchangés.

- Nouvelle DB locale : `/home/lolo/dev/msm/reports/TR2-R5-BV-PK-20261031/analysis-unified-v3.sqlite`, 48 439 296 octets, SHA `5fccabd3775ad9ee5f7fd8dec6ff064805916d6740b8bb8c29bb425a10132799`.
- Original v1 : SHA `e436de69e55e2126c029acc5afea9cb058b0a6fc6ce857e41bc70a26693646c5`.
- Original v2 : SHA `471ca360f35ef5e72fb1c510ce8e6118c31f4a04de0e88e367e2dcad86407a84`.

La copie migrée est justifiée par les champs fréquence/axe/crête/statut distincts
des RMS. Future unification : v3 pour PK+1–80, v1 pour 2–25/PSD, jointure par
campaign_sha et analysis_id, sans duplication des XYZ. Aucune DB LIVE ouverte.

## Livraison, état Git et suite

[Page hors ligne et deux figures](preuves/TR2-20261031-R5-BV-PK-ALTERNATIF-6H/index.html),
[synthèse lisible](preuves/TR2-20261031-R5-BV-PK-ALTERNATIF-6H/SYNTHESE_POC.md),
[preuves et réserves](preuves/TR2-20261031-R5-BV-PK-ALTERNATIF-6H/README.md),
[méthode et code](../../../experiments/r5_bv_pk/METHOD.md).
CSV agrégés, requêtes, SHA et preuves compactes sont publiables ; les fichiers
TR2, DB, checkpoint d'agrégats et PDF restent locaux. Chemins précis dans
LOCAL_ARTIFACTS.json. Les deux figures ont été inspectées visuellement.

Travail dans `MSM_vibe-R5-BV-PK`, branche locale `codex/r5-bv-pk`, publication
explicite vers main. Worktrees historiques et deux stashes 6251b988/f3b829e6
préservés ; le dépôt principal historique n'est pas réinitialisé. Checkpoints
publiés f57ebcdf et 7ed32c4e. Le journal ETAT_COURANT_TR2 est actualisé avec les
résultats, réserves et prochaine décision. Mission, gouvernance et archives
historiques inchangées. Aucune opération STM32, flash/debug, COM, SD physique,
service Windows ou protection de sécurité. Permissions exercées : logiciel,
commit et push normaux vers main uniquement.

**Obstacle scientifique réel :** une amplitude individuelle répétitive compatible
avec BV n'est pas reconstructible de manière suffisamment robuste sur une
part importante du signal réel avec la méthode évaluée. Accélération : zéro
comparaison recevable ; paire globale : zéro minute recevable. Les contre-calculs
et la sensibilité de fenêtre empêchent de lever cette réserve sans affaiblir
les contrôles ou adopter une autre définition non démontrée. Le calcul et la
livraison des `BV_PK_CANDIDATE` sont achevés ; l'objectif de comparaison globale
reste non satisfait. La voie ISO/Wm reste bloquée séparément et n'est pas la
cause de cet obstacle PK.

Suite proposée à ChatGPT : revue indépendante des contrôles, puis décision
explicite sur une définition et une validation de l'amplitude répétitive pour
signaux modulés/multisources. Réutiliser les agrégats et les segments archivés ;
aucune nouvelle campagne physique ni relance automatique n'est nécessaire pour
relire ces résultats. Ne pas assouplir les gardes pour obtenir une classe.

## Publication vérifiée et bilan des gates

Le push normal des résultats a réussi sur main :
`23fee5637ff1a9585dd39caacbbf7dfb6a08b7ca`, message
`R5 PK: publish six-hour candidates, independent controls and safe SQLite cohort`.
Après `git fetch origin main`, **50 fichiers / 5 808 771 octets** ont été relus
intégralement par `git show` depuis cette référence distante et comparés octet
par octet aux fichiers locaux, tous identiques. La mission a aussi été relue
et n'a pas été modifiée. Preuve :
[remote-readback.json](preuves/TR2-20261031-R5-BV-PK-ALTERNATIF-6H/remote-readback.json).
Cette relecture ne remplace pas la revue indépendante ChatGPT.

| Gate | Bilan observé |
|---|---|
| G0 référence | Satisfait : clauses, valeurs, CF et distinction des grandeurs vérifiés |
| G1 scientifique | Estimateur testé ; **comparaison globale indéterminée** sur le signal réel, aucune ACC recevable |
| G2 opérationnel | Archive entière traitée ; métriques et réserves explicites |
| G3 persistance | Migration sans perte, idempotence et requêtes satisfaites |
| G4 lisibilité | Deux figures, HTML, synthèse et données candidates livrés ; aucune classe globale |
| G5 publication | Résultats publiés et relus ; bilan final à publier puis vérifier aussi |

**Statut final scientifique : BLOCKED — méthode PK globale insuffisamment
reconstructible sur cette campagne.** Les tranches de calcul, persistance et
présentation sont terminées ; le livrable candidat est utilisable pour la revue.
La condition de reprise est une méthode répétitive justifiée et contrôlée pour
les composantes modulées/non résolues, pas l'achat Wm ni une manipulation physique.
Aucun test supplémentaire n'est lancé pour masquer cette limite.

Ce bilan final et la preuve de relecture font l'objet d'un dernier commit normal,
également contrôlé après push. Son SHA et l'état Git final sont fournis dans la
réponse utilisateur, sans tenter d'inscrire le SHA d'un commit dans lui-même.
