---
mission_id: TR2-20261103-SHIELD-V12-PRECOMMANDE
status: IN_PROGRESS
base_ref: main
base_sha: ebae78d21f1106ce0a4554f1399012478f8ffb59
initial_head: c32ca3ec43eadfd18df5191a9f3b43bcf1511574
result_sha: 55cfcf2c0eea9a158117c47f5af84b0fc740fdf1
created_at_utc: 2026-11-03T16:16:14Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Shield V1.2 — dossier de revue produit, fabrication non libérée

Le candidat KiCad et CAM est prêt pour revue documentaire et comparaison gabarit. **BLOCKED_FABRICATION / NE PAS COMMANDER** : les ZIP officiels ST restent inaccessibles et la correspondance mécanique C05/engagement J5–Zio n'est pas démontrée. Ni le détrompage automatique abandonné ni le boîtier final ne constituent un blocage. Le statut provisoire IN_PROGRESS porte uniquement sur la publication/relecture distante en cours ; la limite technique est décrite ci-dessous.

## Référence et conservation

Départ distant `c32ca3ec43eadfd18df5191a9f3b43bcf1511574`, contrat et décisions03/11 lus ; gouvernance V2-B3 et journal ETAT_COURANT_TR2 consultés. Travail isolé dans `/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-V12-PRECOMMANDE`, branche locale `codex/shield-v12-precommande`, publication autorisée vers `main`.

Le dépôt principal est resté au commit historique `82169af7e76797731358852bec83bfb0ceb54ccf`, propre mais divergent ; aucun pull/reset/merge/rebase dessus. Worktrees antérieurs intacts. Deux stashes conservés : `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87`. Ancienne archive V1.1 non écrasée ; contrat V1.2 copié dans sa nouvelle archive. Aucun changement de gouvernance ou de décision humaine.

## Résultats et modifications

Dossier : `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/` ; noms de projet historiques conservés, cartouche/sérigraphie1.2.

- **J5 défini nominalement** : mâle inférieur Harwin M20-9990245 ; **femelle M20-7820242 à ajouter sur chaque Nucleo**, sur la dernière paire CN12-69/70, en travers des deux rangées. PG4/69→DE, PG6/70→NC ; aucune liaison entre les deux. UM2861Rev4 table19 p37 réellement inspectée, cohérente avec la numérotation du contrat Rev10 ; pas de PDF Rev10 prétendu retéléchargé. Hauteur femelle8,50±0,15mm, pin6,10, isolant2,54 ; empilage nominal11,04 entre dessusNucleo/dessousshield, **à confronter aux Zio C05 et trous/soudure/dégagement réels**. Voir `docs/J5_ET_EMPILEMENT_V1_2.md`.
- **Pièces réelles identifiées** : sockets12 M20-7821242 affichées disponibles2275pces/1,93USD à l'unité ; sockets9 et femelles2 également sourcées. J1/J2/J3 famille998 aux tailles exactes ; J4 M20-9982045 recoupé2×17, variante1745 non retenue d'après liste fabricant. Prix/stock ne sont ni réservation ni devis. Vis nylon M2,5 candidates indiquées ; hauteur finale entretoises/longueurs et stock de toutes les autres pièces restent à figer avant achat.
- **Corrections minimales CAO** : courtyards sockets agrandis selon enveloppes/tolérances Harwin, trous connecteurs1,02mm (tolérance trous finis±0,05 à demander au devis). C1 Kemet C315C104K5R5TA au pas2,54 ; C2 Panasonic ECA1HM100 **sans suffixeB** au pas2,00,Ø5 nominal/5,5 réservé,50V. Une branche3V3 TP1–C2 écartée du nouveau padGND après DRC ;31autres trajets conservés. Aucun net fonctionnel, position J1–J9, ancrage ou contour changé.19empreintes,187pads dont11NPTH,59broches connectées,4vias.
- **Fixations préservées** : trois ancrages Nucleo, deux par FRAM/SD, quatre pour étriers J8/J9. Têtes **et rondelles** NucleoØ≤4,5mm :Ø5 laissait seulement0,04mm nominal au corps Zio inférieur ;Ø4,5 laisse0,29mm nominal au corps/0,04 au courtyard majoré. Cette déduction sur le modèle nominal ne remplace pas C05 réelle. Deux barrières de détrompage retirées du SCAD/STL, aucune retenue positive supprimée. Patins/hauteurs d'étrier restent réglables aprèsPCB, sans pression sur composants/contacts ; aucune pièce imprimée.
- **FRAM3V3** : schéma Eagle Adafruit réel inspecté : VIN→AP2112K-3.3→FRAM/4050 ; MISO direct, sortie régulateurJP1-8NC. Le guide Adafruit supporteVIN/logique3V3. Pas d'injection5V ni de pont au LDO. Dropout, rail interne et marges dynamiques non mesurés ; qualification électrique réelle ultérieure, pas prétendue acquise. Voir `docs/ALIMENTATION_FRAM_V1_2.md`.
- **Interfaces** : SD fente-X, RS485+X et reprise d'effort externe, nappe-Y ; cuivre shield arrêté avant domaine isolé du Click. Aucun netGND2/A/B/5V sur shield. GéométrieClickv101 et schémav102 ne sont pas assimilés. Nappe/borniers/CN15/ST-LINK/USER/RESET restent des accès à conserver. Boîtier/LED/poussoir finaux différés.

## CAO ST — faits et limites

Page primaire : https://www.st.com/en/evaluation-tools/nucleo-u575zi-q.html. Liens MB1549-BDP4.0 et Manufacturing3.4 publiés, **versions de paquets uniquement**. Les deux URL officiels ST et leurs homologues STChine expirent par HTTPS (~18s). curlWindows expire ; Edge à profil temporaire propre renvoie ERR_HTTP2_PROTOCOL_ERROR sur ZIP et schémaC05 ; repli protocolaireHTTP/1.1 normal également en timeout. Aucun certificat/protection/authentification contourné.

**Aucun ZIP reçu : contenu interne, formats, révision C05/C02/C03 et SHA des archives inconnus.** Les hashes des deux dessins Harwin réellement reçus figurent dans `docs/sources-v1-2.json` ; sources Adafruit/Click et hashes antérieurs conservés. L'outil documentaire ne décode pas les ZIP et n'en a pas vérifié le contenu. Logs publics réduits dans `docs/checks/v12/`, originaux locaux `/home/lolo/dev/msm/reports/TR2-shield-v12-20261103`. Aucune cote C05 inventée à partir d'une photo ni assimilée à une autre révision.

## Validations réellement exécutées

| Contrôle | Résultat / preuve |
|---|---|
| Génération schéma/PCB, application32trajets et recalcul zones | succès, `docs/checks/rebuild-v12.json` |
| ERC KiCad10.0.6 `--severity-all --exit-code-violations` |0violation, `ERC.json/log` |
| DRC `--schematic-parity --all-track-errors --severity-all --exit-code-violations` |0violation,0non-connecté,0parité ; `DRC.json/log` |
| Projet strict préservé | octet pour octet identique à la base : minclearance0,20/mintrack0,25/minvia0,60 ; aucune sévérité/exclusion modifiée |
| Netlist XML↔PCB↔attributsX2 des deux cuivres | PASS, `export-verification.json` |
| Tous centres/diamètres PTH/NPTH/vias et contour CAM | PASS, même comparateur |
| Face dessous J1–5, J5PG4/PG6, perçages et pas réels,31trajets conservés | PASS nominal, `preorder-native.json` ; aucun fit physique déduit |
| Retenues SCAD modifiées | deux compilations OpenSCAD `--hardwarnings`, sortie0 ;5maillages fermés/connectés, volumes positifs, `mechanical-verification.json` (trois modèles inchangés réutilisés) |
| Gabarit PDF3pages, A4, échelle1:1, barres100mm X/Y | PASS génération et réouverture PDF, page1 inspectée visuellement ; `preorder-template.json` |
| Hashes artefacts |68fichiers CAD/exports/mécaniques relus et vérifiés ; `artifact-hashes.json` |
| Gitdiff et périmètre | contrôlés avantcommit, aucun firmware/archives antérieures inclus |

Erreur d'outillage traitée : `pcbnew.SaveBoard` réécrivait des règles projet aux défauts ; détection par assertion mécanique, correction des deux générateurs pour préserver le fichierprojet, puis exécution effective des contrôles stricts. Le DRC strict a ensuite détecté deux distances insuffisantes au nouveau C2 ; preuve conservée `checks/v12/DRC-C2-before-fix.json`, correction locale3V3 et DRC relancé avec succès. Aucune erreur masquée ni règle réduite. Les cinq catégories DRC déjà ignorées par le projet restent identiques à la base et sont listées dans `scope-and-project-audit.json` ; aucun contrôle supplémentaire désactivé.

Normalisation des fins de ligne générées Windows→LF et espaces de fin insignifiants ; contrôles CAM reparsés ensuite. Cela ne remplace pas les contrôles natifs. Aucun `tr2_validate.sh`/cross-build firmware lancé : aucune source firmware modifiée, validation pertinente = CAO/CAM, pas essai physique ni qualification électrique/vibratoire.

## Coût, sortie et prochain contrôle minimal

BOM et budget révisés : scénario **91,42EUR pour deux assemblages/lot5PCB**, modules/blocs5V réemployés, hors boîtier complet. Hypothèse comptable1USD=1EUR et réserveTVA20%, frais provisionnels, **aucun cours/devis livré**. Coût50EUR non démontré, ne pas reprendre53,27EUR historique. Aucun achat ou commande.

`exports/gabarit-C05-V1_2.pdf` : pages1/2 pour chaqueC05, page3 dessous miroir ; `docs/comparaison-C05.csv`105centres avec colonnes constructeur **vides**. États distincts : **CAD_VERIFIED pour les contrôles électriques/CAM du candidat**, **READY_FOR_PHYSICAL_TEMPLATE_CHECK**, **BLOCKED_FABRICATION** pour la libération. Ne pas les fusionner en « fabricable ».

Une suite physique autorisée ou les ZIPST accessibles normalement doit démontrer : centres/contour/trois ancrages des deuxC05 ; paireMorpho69/70 libre/soudable et hauteur/engagement simultané sans flexion ni collision ; contoursMKI/Click réels compatibles avec les zones ancrages/isolation. Le gabarit et une vue avec règle suffisent pour la partie XY, **pas pour les hauteurs**. Aucun nouveau relevé de l'entraxe15,2 ou GNDMKI demandé. Détrompage, boîtier, patins définitifs, guidesLED et poussoir ne bloquent pas les PCB nus. Aucune manipulation demandée dans la mission présente.

## Journal, Git et publication

Journal ETAT_COURANT_TR2 **actualisé** : acquis CAO V1.2, J5 à ajouter, pièces/vis, budget, limites ST/C05 et suite minimale. Les historiques de campagne restent intacts ; aucune nouvelle conclusion vibratoire. Toutes les permissions exercées sont documentaires/logiciels, commit et push normaux autorisés ; flash/debug false respectés, aucun accès matériel/média.

Commit CAO évalué `55cfcf2c0eea9a158117c47f5af84b0fc740fdf1` — `SHIELD V1.2: verify candidate CAD and define J5 pre-order integration`. Rapport provisoire préparé pour push explicite vers `refs/heads/main` ; relecture distante et statut final à consigner après publication. Le dépôt principal/stashes restent préservés ; aucun forcepush/merge/rebase. Ce rapport ne vaut ni qualification physique ni autorisation d'achat.
