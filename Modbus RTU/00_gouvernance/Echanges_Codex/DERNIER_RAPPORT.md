---
mission_id: TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE
status: DONE
base_ref: main
base_sha: f99e85cc3857aa6dd9db60b16c4a37ba1c7550b0
initial_head: 44e0569e4241c54daab30bf9abd8b06de2ce2320
result_sha: 729416af1a01772783026c3961e79371b35cb031
created_at_utc: 2026-10-31T13:56:12.519555+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Préqualification hors ligne — faisabilité limitée, conserver ±8g

**Les calculs, tests, revue FS, figures et persistance sont terminés et les
livrables ont été publiés puis relus intégralement.** Le verdict
métier est **FAISABILITÉ LIMITÉE** : l'énergie globale est calculable et reproductible
sur les six heures modulées, sans rechercher une raie stationnaire ; la chaîne
physique et la définition humaine/normative de la pondération restent à qualifier.
Aucun score, seuil, grade ou calcul de confort ISO/Wm/BV n'est produit.

**Recommandation FS : conserver ±8g sur ce montage.** Des sorties archivées
atteignent +7,676728g nominal sur Z. La cause physique est indéterminée ; ces
transitoires empêchent de recommander aujourd'hui ±2g ou ±4g sans qualification.
Aucun firmware, registre, matériel, SD, COM ou service Windows n'a été touché.
Aucun achat, flash/debug, nouvelle acquisition ou reconfiguration de traversée.

## A — Résultats numériques et portée

Lecteur strict R3, décodage/streaming/qualité R5 réutilisés sans modification.
16 méthodes : quatre bandes 1–2/2–5/5–25/25–80Hz, total plat1–80Hz, filtre test
LP10 et variantes basses0,8/1,2Hz, chacune sous hypothèses MCU/QPC estimées.
Le LP10 est défini `H(s)=1/(1+s/(2*pi*10))` derrière le passe-bande, bilinéaire :
**test de sensibilité**, pas modèle de perception humaine. Coefficients, phase,
amplitude, hashes et définitions publiés. Aucune formule normative conjecturée.

| Max RMS XYZ, mg nominaux, MCU | P5060s | P9560s | Max60s | P50120s |
|---|---:|---:|---:|---:|
| Plat1–80Hz | 31,137 | 36,789 | 51,410 | 31,436 |
| TestLP10 | 7,245 | 12,507 | 16,173 | 7,282 |

Ces valeurs ne sont pas un jugement de confort. 365 fenêtres FULL à60s et182
à120s par méthode, 8 816 lignes d'axes/direction/norme technique au total.
Les premières et dernières fenêtres partielles/invalides restent identifiées.
Énergies de la **période entière valide**, incluant la dernière fenêtre partielle :
58 525 510 XYZ pour MCU après warmup/settling, RMS XYZ plat12,376/9,470/27,643mg,
max directionnel27,643mg, norme technique31,733mg ; test3,924/2,960/7,313mg,
max7,313mg/norme8,811mg. Pas de moyenne arithmétique des RMS ni de remplissage.

Les quatre filtres se chevauchent : parts moyennes de **leur somme**0,075% /
0,144% /24,803% /74,978%. Leur somme d'énergie / total varie0,990–1,117,
exportée par fenêtre ; ce ne sont pas quatre fractions orthogonales de la variance.
DC/gravité conservée dans l'archive, supprimée par passe-bande dans les métriques.
Une marche d'offset ou une inclinaison variable peut contribuer à l'énergie en
bande ; aucune séparation physique magique. Température non archivée.

**Même contenu temporel :**182 paires alignées par méthode, reconstruction RMS120
à partir des deux énergies60 exacte à4,5e−16 relatif. Max60/RMS120 médian1,0167,
P951,0985, max1,3039 pour le test ; rang Spearman0,9847 (plat0,9804).
Ces écarts proviennent aussi de la variation dans le temps : le maximum des deux
minutes n'est pas la moyenne énergétique de deux minutes. Distributions60/120
et comparaisons fenêtre par fenêtre restent séparées ; populations FULL globales
légèrement différentes aux bords, explicitement signalées.

Sensibilité de borne basse0,8/1/1,2Hz : changement du RMS plat <0,06% par minute
sur cette archive dominée par les bandes supérieures. Ce n'est pas une preuve
physique à1Hz. Rapport test/plat0,206–0,392 : pas de conversion universelle.
Alternative de cadence QPC : plat −0,873% à−0,069% par minute ; voir exports pour
les autres méthodes. MCU2661,462Hz et QPC2679,516Hz sont des hypothèses, pas SI.
L'axe temporel est MCU relatif, étendue21999,989s ; l'ancien témoin QPC démontre
un intervalle intérieur de21601s, sans transformer nos timestamps en UTC connue.
La répétabilité physique entre deux poses/campagnes n'est pas démontrée.

## B — Pleine échelle : données, déduction, limites

**Source/code :** recorder c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e,
header logique8g/244µg/count. Le code appelle fifo_live_start(...,fs=2), mappe8g,
configure puis impose un readback0xAC au START. Court historique précédant le
long : ctrl1=172 réellement observé. Le registre du long n'a pas été récupéré
après coupure ; **FS_PHYSIQUE_NON_VÉRIFIÉ sur toute sa durée**. Les fichiers
recorder/driver/FIR sont identiques au SHA source archivé (diff Git vérifié).
Ne pas confondre cette chaîne du TR2-002 avec l'ancien AC1 du principal bloqué.
Aucune lecture actuelle du matériel dans cette mission.

MESURÉ sur sorties filtrées/décimées, conversion nominale244µg/count :

| Axe | Min g | Max g | Marge ±2g | Marge ±4g | Marge ±8g |
|---|---:|---:|---:|---:|---:|
| X | −0,931836 | 1,258064 | 0,741936 | 2,741936 | 6,741936 |
| Y | −1,258552 | 0,700036 | 0,741448 | 2,741448 | 6,741448 |
| Z | −4,631120 | 7,676728 | −5,676728 | −3,676728 | 0,323272 |

Moyenne Z1,018618g et quantile99,9%1,160464g ne bornent pas les extrêmes.
41 sorties Z dépassent2g entre17613,441565 et17614,769104s MCU ; cinq dépassent4g,
deux6g, une7g. Pic index46876121,17613,446449s. On ne sait pas attribuer cet épisode
à un choc, une perturbation électrique/SPI ou une autre cause.

ESTIMÉ avec le FIR réel107Q15 (L1=1,4486083984375), arrondi±0,122mg : au moins
5,299297g nominaux parmi ses entrées numériques contributrices **si la chaîne
est correcte**. Le dépassement FIR ne justifie donc pas une réduction automatique
à±4g. Cela n'établit pas une accélération analogique réelle ni le pic natif exact.
SOURCE_RAIL=0/OUTPUT_CLIP=0 : le premier couvre les rails numériques exacts dans
l'historique FIR contribuant à une sortie, le second son clamp int16. Ni l'absence
de saturation analogique ni les crêtes natives perdues/queue non émise au STOP
ne sont prouvées. Pas de conclusion depuis les seuls RMS de bande.

SPÉCIFICATION FABRICANT : [ST DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf),
tableau3 note6 et tableau30 : pas0,244/0,122/0,061mg pour±8/4/2g, bruit typique
XY75/Z110µg/√Hz indépendant du FS, caractérisé100Hz–6,3kHz, pas une garantie à1Hz.
Borne d'arrondi±0,122/0,061/0,0305mg ; bruit uniforme de quantification hypothétique
0,07044/0,03522/0,01761mg RMS. Densités théoriques entrée/sortie et limites dans
[FS_AUDIT.md](../../../experiments/r5_prequal_offline/FS_AUDIT.md).

Simulation distincte sur un analogue **connu avant quantification**, trois niveaux,
avec/sans bruit blanc explicitement hypothétique, FIR nominal puis arrondi de sortie.
À0,3mg sans bruit : biais plat+2,35% en8g/−0,066% en2g ; à3mg≈0,011%/0,008%.
Avec bruit, le gain n'est pas monotone. Résultats RMS/spectre/vitesse conservés.
Aucune information perdue n'est recréée en requantifiant l'archive ; aucun gain4×
de SNR physique annoncé. **Garder8g ici ; étudier4g/2g seulement après qualification
instrumentale et des transitoires**, dans une autre mission matérielle.

## Tests, provenance, persistance et validation choisie

**10 tests ciblés PASS**, commande réellement exécutée :

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -W error::ResourceWarning -m unittest discover -s experiments/r5_prequal_offline -p 'test_*.py' -v
```

Oracles analytiques indépendants, sinus0,3/0,5/0,8/1/1,2/2/5/10/20/40/80/100Hz,
multitons, AM/FM/impulsions, DC/gravité/marches, invalidités/gaps/clips,
découpage, quantification, refus de perte de DB/artefacts et idempotence.
144 contrôles réels sur trois périodes faibles/médianes/fortes, chacune120s et
ses deux60s,16méthodes : FFT+transfert/Parseval vs causal temporel, tous PASS,
écart RMS relatif max2,81e−10 sous tolérance fixée0,001. Pas de filtre affaibli.

Parcours réel réussi au premier essai,134,950s, RSS377504KiB≈368,7MiB ; blocs
et états par seconde bornés, contrôles sur trois contextes courts. Le code numérique
évalué est au checkpoint729416af (empreintes core/runner dans MANIFEST), inchangé
ensuite. Les autres modules rendent/interrogent les agrégats, simulent la résolution
ou auditent les extrêmes. Aucune ancienne suite/FFT PK ou analyse RMS historique
relancée. Pas de tr2_validate/ARM/.NET : ces systèmes ne sont pas modifiés.
Deux figures principales et une réponse de filtres inspectées/exportées ; page HTML
locale, CSV, note de décision courte et requêtes reconstructibles.

Source locale immuable :
`/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2`.
SHA entier avant/après91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5,
CRC/indices/session/footer stricts,58 552 136XYZ/351 312 816B utiles ; SHA utile
5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41.
Queue préallouée exclue. R3 reste CLOSED_WITH_RESERVATIONS : digest RAM indépendant
perdu, intégrité interne seulement. Sensibilité, pose double-face, axes et horloges
non étalonnés, UTC_UNKNOWN. Réponse FIR1–100 réutilisée depuis preuve précédente,
coefficientSHA0e528c6d985207c265ccaa4a5df797ccdfa5ccfe938ad9a51a6a6ad8c94cc1b4.

SQLite : copie unique dérivée v4 justifiée par l'interdiction de modifier v1/v2/v3.
Les12tables historiques sont conservées sur toutes leurs lignes/schema, dont2204RMS
et30804PK. Une campagne,16méthodes et8816nouvelles lignes ; double import et audits
idempotents aussi en octets, integrity_check=ok/FKvide, cinq requêtes exécutées
(comptes1/16/32/8816/1). Treize artefacts de résultats/audits référencés.
DB locale `/home/lolo/dev/msm/reports/TR2-R5-prequal-20261031/analysis-unified-v4.sqlite`,
54 837 248B, SHA87f79f2e6cc77845baa3b5a24118ab0756613d1e66e612976648059553d6aa79.
SHA originaux v1 e436de69…93646c5, v2 471ca360…6407a84, v3 5fccabd3…132799
inchangés avant/après ; empreintes complètes et inventaire dans sqlite-proof.json.
Pas de XYZ bruts ni DB/TR2/PDF/NPZ transférés sur GitHub ; aucune DB LIVE ouverte.

Échecs logiciels corrigés et traces conservées : collision d'import catalog avec
l'ancien module (nom propre prequal_catalog), connexions SQLite non fermées
(fermetures explicites), découpage SQL sur commentaire contenant un point-virgule
(commentaire corrigé, vraies requêtes rejouées). Aucun recalcul6h nécessaire.

## Décision et suite

[Note POC pour décision](preuves/TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE/NOTE_DECISION.md),
[figures et HTML](preuves/TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE/index.html),
[preuves/chemins/SHA](preuves/TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE/README.md).

L'achat des normes peut apporter une définition de pondération humaine ; cette
préqualification démontre seulement que son calcul large bande est techniquement
réalisable. Avant de dépenser pour une mesure exploitable, cadrer l'indicateur
et les éditions, accepter la qualification du montage/gain/bruit1–100/cadence,
et investiguer la crédibilité des transitoires. Aucun achat ni verdict réglementaire.

Protocole futur documenté : mêmes pose/axes/horloge/FIR, readback FS au START et
périodique/fin, bruit au repos+température, référence connue, bruts natifs autour
des événements, paliers bornés. Deux capteurs synchrones préférables ; alternance
explicitement non simultanée. Une mission STM32 distincte devra autoriser flash/debug.
Aucune intervention utilisateur nécessaire pour clore cette préqualification.

Travail isolé MSM_vibe-R5-prequal/codex/r5-prequal-offline depuis44e0569e ; dépôt
principal historique82169af, anciens worktrees/stashes6251b988/f3b829e6 préservés.
Mission/gouvernance/archives historiques, firmware et supervision inchangés.
Journal ETAT_COURANT_TR2 actualisé : faisabilité limitée, pic/marges FS, preuves,
prochaine qualification et réserves historiques. Checkpoint729416af déjà publié.

G0 source/préservation satisfait ; G1 calcul non normatif et contrôles satisfaits ;
G2 revue FS et recommandation conditionnelle satisfaites ; G3 persistance et
exports satisfaits ; G4 note de décision satisfaite ; **G5 résultats publiés et
relus intégralement**. DONE clôt uniquement cette préqualification à faisabilité
LIMITÉE : ni calcul ISO certifié, ni autorisation de réduire le FS ou de lancer
une acquisition. La revue indépendante ChatGPT demeure distincte.

## Publication vérifiée

Checkpoint code : `729416af1a01772783026c3961e79371b35cb031`, message
`R5 prequalification: checkpoint tested global energy method and FS source audit`.
Code supplémentaire, résultats, figures et bilan :
`e211909bb552ce10c358bf7049e5fed292980cd9`, message
`R5 prequalification: publish six-hour energy feasibility and evidence against blind FS reduction`.
Les deux push normaux ont réussi sur main. Après récupération de main depuis
GitHub, **51 fichiers /4 292 315 octets** ont été relus intégralement depuis
la référence e211909b et comparés octet par octet aux fichiers publiés : tous
identiques, mission comprise, sans modifier celle-ci. Preuve :
[remote-readback.json](preuves/TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE/remote-readback.json).

Le présent statut DONE, le journal final et cette preuve sont publiés dans un
dernier commit normal, vérifié lui aussi après push. Son SHA est fourni dans la
réponse finale avec l'état Git, sans tenter de l'inscrire dans son propre contenu.
Travail local propre après publication attendu et à contrôler ; deux stashes et
worktrees historiques préservés. Aucun effacement/reset/rebase/force push.
