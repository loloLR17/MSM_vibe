---
mission_id: TR2-20261015-R3A-ACQUISITION-LIVE-ARCHITECTURE
status: DONE
base_ref: main
base_sha: 3f3ca1c38742c277de349b789e3e82fd52a1c791
initial_head: c5c1c22df5291dc29782d4a81cc3dc17adf56556
result_sha: 0c6f002fd9cc09f577ff9ec446b5af46d5393c27
created_at_utc: 2026-10-15T21:43:18Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-A — acquisition continue et revue architecturale

**Critères logiciels R3-A satisfaits ; rapport et preuves publiés puis relus sur main.**
Un propriétaire continu réutilise le driver ST, le transport SPI STM32 qualifié
et les guards/parser FIFO existants. Le vrai moteur R2 utilise ce propriétaire
dans un test d'intégration avec SPI/FRAM simulés. **La production reste V1 ; aucun
profil2, B3 valide ou PASS physique n'est annoncé.**

## Référence et préservation

Mission READY récupérée sur main `c5c1c22`, base3f3ca1c vérifiée ; gouvernance
inchangée depuis la mission précédente (diff contrôlé), journal intégral et
références R0/R1/R2 consultés. Le contrat n'a pas de created_at_utc ; aucune date
contractuelle inventée. Dépôt principal propre historiquement divergent à82169af,
aucun pull/merge/rebase/reset. Travail dans
`/home/lolo/dev/msm/projects/MSM_vibe-R3A-live-acquisition`, branche locale
`codex/r3a-live-acquisition`, destination autorisée main. Les worktrees et stashes
6251b988e8b20f97997a8f98fd26d4d35a45a1b2 /
f3b829e6188f491920edf74a5f9ed0f559d08b87 sont préservés. Fichiers protégés
recomparés par SHA256, gels/dettes/archives antérieures inchangés ; mission et
rapport R2 archivés intégralement depuis3f3ca1c.

Code évalué **`0c6f002fd9cc09f577ff9ec446b5af46d5393c27`** —
`R3-A: reuse qualified FIFO guards for a continuous causal acquisition owner`.
Les commits suivants concernent uniquement preuves/rapport/journal/roadmap.

## Développement et architecture

- `fifo_live` : tampon constant896octets, continuité tags/compteurs/timestamps
  conservée, totaux et temps64bits ; pas de limite de capture/durée/campagne.
  Un burst passe parser, statut et readback configuration avant callback ; aucune
  publication partielle sur échec. Gap de poll, absence de données, overflow,
  erreur SPI, tag, configuration ou consommateur provoquent arrêt sûr.
- `live_fifo_acquisition` : vrai port causal R2 START/STOP/observe. Configuration
  et run figés, lectures effectives des contrôles avant observation ; STOP
  OFF/BYPASS/timestamp OFF et readback indépendants de la FRAM. Init sans I/O,
  aucun auto-START ; budget I/O absent/refusé par défaut.
- `stm32_live_acquisition` : réutilisation du même transport SPI3/CS/UID/clock et
  contrôle HAL/preclose. Claim unique par boot, refus d'un second attachement.
  Recyclage des logs finis entre quanta validés, jamais des erreurs sticky ;
  budget cleanup conservé. Aucun driver ST/HAL/pin modifié.
- Mutualisation démontrée : corps du parser et contrôle FIFO identiques avant/après,
  exportés depuis fifo_window et utilisés par les deux chemins. Aucun parseur
  recopié, aucun framework, aucun chemin V1 supprimé.

Cartographie complète, méthode et matrice **conservé/mutualisé/remplacé/différé** :
[`LIVE_R3A.md`](../../05_Firmware/LIVE_R3A.md).
Inventaire407 modules/interfaces/tests avec preuves CMake et symboles :
[`r3a-module-inventory.json`](preuves/TR2-20261015-R3A/r3a-module-inventory.json).
L'ELF canonique possède des symboles issus de88 chemins du dépôt (incluant
sources partagées/vendor/générées identifiées) ; bibliothèque compilée et unité
réellement liée sont distinguées. R1/R2/nouveau propriétaire sont préparés/testés,
pas qualifiés « morts ». Le B5 V1 et le moteur epoch restent distincts pour leurs
contrats/records ; un seul média/driver FRAM est réutilisé. **Aucun candidat à
retrait immédiat démontré** ; les archives/bancs historiques sont nécessaires.

## Mesures de code et images

Méthode reproductible : script commité `tools/measure_live_architecture.py`, blobs
Git b6bae880/3f3ca1c/0c6f002, lignes physiques incluant blancs/commentaires.
Code propriétaire C/C++ firmware et expériences, hors vendor/générés/builds ;
tests source séparés ; documentation MD/RST/ADOC. JSON/CSV/logs/preuves ne sont
pas des LOC ; la colonne code n'est pas limitée aux fonctions liées à l'ELF.

| Référence | C/C++ propriétaire | Tests | Documentation |
|---|---:|---:|---:|
| Avant R1 b6bae880 |30415|45422|140951|
| HEAD R2 3f3ca1c |31768|46516|141714|
| Code R3-A 0c6f002 |31999|46620|142059|

R3-A versus R2 : **+231 code, +104 tests, +345 documentation**, cette dernière
incluant archive et évolution de mission entrante. Ce ne sont pas747lignes de
production supplémentaires. Diff de notre tranche c5c1c22→0c6f002 :
**757ajouts/10suppressions Git**,17fichiers, incluant outils/docs/archive/tests ;
numstat conservé séparément. Les commits d'échange ultérieurs ne sont pas comptés
dans la mesure du commit de code. L'outillage Python ne devient pas C/C++.

| Image/mesure réelle | text | data | bss | Portée |
|---|---:|---:|---:|---|
| ELF R2 conservé, empreinte vérifiée |170892|480|602232|V1 canonique, code59d3757 inchangé à3f3ca1c|
| Canonique R3-A |170916|480|602232|V1, nouveau propriétaire non appelé|
| Lien forcé R3-A distinct |175144|480|602232|Nouveaux symboles résolus, aucun nouvel objet propriétaire instancié|

Avant R1 : pas de taille image remesurée ; comparaison LOC seule disponible.
Propriétaire ARM1064octets dont896de burst, adaptateur incluant propriétaire1088,
hors contexte STM32/consommateur. Cadres statiques `.su` conservés, pas une borne
complète de pile/IRQ ni de CPU. Aucun gain de marge physique n'est extrapolé.

## Tests et erreurs

Preuves et empreintes : [`TR2-20261015-R3A`](preuves/TR2-20261015-R3A/README.md).

- **131/131 tests hôte PASS**,35,60s au dernier cycle ; full validation puis
  rebuild/tests finaux justifiés par le claim exclusif ajouté au transport.
  Les tests bornés V1, FIFO et budgets existants restent actifs.
- **Port R2 réel + owner réel sur SPI simulé** : START/STOP avec readbacks,
  consommation, APPLY refusé sans I/O pendant acquisition, arrêt de sécurité
  après panne FRAM ; aucune observation de succès inventée par un stub.
- **Plus de4millions XYZ modélisés**, quatre pleines échelles, au-delà des limites
  de capture précédentes ; frontière temps32bits franchie. Il ne s'agit pas d'un
  essai à cadence physique TR2 ni d'une preuve d'endurance.
- Erreurs de FIFO/tags/parité/saturation/timestamp/configuration, absence de données,
  gap, backpressure consommateur et40positions d'injection START I/O contrôlées.
  La fixture avait initialement laissé une injection START tomber dans STOP ;
  sa portée a été corrigée sans retirer l'assertion de STOP vérifié.
- **ASan/UBSan PASS** sur propriétaire/port/intégration R2 hôte. Les corps partagés
  sont byte-identiques (preuve shared-bodies.json), les oracles historiques inchangés.
- **Cross-build cumulatif ARM PASS**, GCC14.2.1/SDK1.9.0. Nouveau propriétaire et
  binding STM32 compilés ; lien forcé des points d'entrée dans une image distincte,
  sans flash, car le canonique V1 ne les appelle pas. Premier contrôle de lien a
  montré que le constructeur du port était éliminé ; l'ancrage explicite de ce
  symbole a ensuite démontré le lien complet attendu, sans changer le produit.

Commandes/logs réels conservés : `bash ./tr2_validate.sh` avec SDK Linux, rebuild
hôte+ctest puis `--cross-build-only`, build/exécution sanitizer et commande de lien
forcé. Validation complète requise par modification partagée/transport, pas une
répétition de qualification physique antérieure.

BIN canonique non flashé SHA256
`e8bb8fa797e7c3865de9a10f966a1045389644fcf65b5a78e0242c3a54704e1d` ;
ELF `8bcf27df8e2140a26a6c86fd3510e78fd3d75f70988fe6d3642eafdd90be8134`.
Image de lien forcé **NON DESTINÉE AU FLASH** SHA256
`285b43d540f22f91047674a610acfb7de3dc64c12852a6f0a09ef68f6ee2fe7f`.

## Limites et suite R3-B/R4

1. **Risque fort R4** : commits/relectures FRAM synchrones R2 après START peuvent
   affamer FIFO. Le port ne donne aucun budget vrai par défaut. Garantir STOP,
   service/latences avant composition réellement active ; le service continu
   seul ne corrige pas ce coût de persistance.
2. **R3-B** : fenêtres exactes, unités/RMS/crêtes/alarmes/qualité B3, projections
   B1/B6/B7 et routage RTU2/effets hérités à compléter ; production reste V1.
3. **Qualification** : politique gap, SPI électrique et CPU/stack sous charge non
   mesurés sur cible. Les guards hérités ne prouvent pas une métrologie temporelle
   exhaustive ni zéro perte physique. Toutes les trois composantes sont capturées ;
   masque logique figé appliqué en R3-B, saturation d'un axe non choisi rejetée
   conservativement, à couvrir sans affaiblir V1.

Journal et roadmap mis à jour factuellement ; dettes anciennes non réécrites.
Choix de mutualisation et dette bornée dans LIVE_R3A.md pour contrôle ChatGPT.

## Matériel et publication

Seule action matérielle/outillage : **énumération ST-LINK en lecture seule**,
SN003500463235510B37333439/FWV3J15M7/NUCLEO-U575ZI-Q observés. Pas de lecture UID,
Flash, variables ou accès cible. État actuel de la carte/alimentation/câblage et
marges de coexistence non établis : aucune campagne physique ni flash tenté.
Ce choix préserve l'état hérité non relu ; ne pas le présenter comme une carte
absente ou comme un échec de qualification. Aucun accès FRAM/SD/RS485/reset,
Option Bytes/protections inchangés. Permissions commit/push exercées, flash/debug
non exercées. HP retiré ; le banc futur reste T1000.

Push normal effectué vers main au commit
**`a33066d52c4aa08771a2c78ae19778009e25b1f5`** —
`R3-A: publish software qualification, architecture inventory and provisional report`.
Référence récupérée de GitHub, tous les livrables modifiés relus intégralement et
comparés octet à octet ; 30 fichiers du manifeste, 2332 fichiers protégés,
sources évaluées et deux stashes vérifiés. Reçu détaillé :
[`remote-readback.json`](preuves/TR2-20261015-R3A/remote-readback.json).
Worktree propre après cette publication ; dépôt principal et autres worktrees
préservés. Cette mise à jour DONE clôt les critères logiciels R3-A, sans
qualification physique ni validation indépendante ChatGPT présumée.
Le SHA de cette dernière publication et sa vérification sont fournis dans la
réponse finale ; aucun SHA autoréférent n'est inscrit dans son propre commit.
