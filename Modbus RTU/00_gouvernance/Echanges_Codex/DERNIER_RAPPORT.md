---
mission_id: TR2-20261015-R1-LIVE-FRAM-CONFIGURATION
status: IN_PROGRESS
base_ref: main
base_sha: null
initial_head: b6bae88001a46527ccab5249fce0c2527f2672d2
result_sha: f4697febfd5cea267a3c4fa21a1f1e0071845665
created_at_utc: 2026-10-15T10:23:14Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R1 — configuration LIVE et autorité FRAM indépendantes de la SD

**Résultat logiciel acquis ; publication et relecture distante encore à terminer.**
Backend R1 implémenté et testé sur les vraies implémentations de stores, avec
transport physique simulé. Ce résultat ne qualifie ni LIVE opérationnel, ni le
matériel, ni les marges temps réel. Aucun blocage technique rencontré dans le
périmètre logiciel ; R2/R3 et provisioning physique restent des étapes distinctes.

## Référence et préservation

Mission READY lue sur main `b6bae88001a46527ccab5249fce0c2527f2672d2`, avec
AGENTS.md V2-B3, protocole V2, gouvernance §10, journal intégral, contrat/norme R0,
mapping et oracles. Le contrat ne fournit ni `base_sha` ni `created_at_utc` :
`base_sha: null` ici, aucune valeur contractuelle inventée. L'initial_head est le
HEAD distant effectivement récupéré et utilisé.

Le dépôt principal reste à `82169af7e76797731358852bec83bfb0ceb54ccf`, propre mais
historiquement divergent ; aucune fusion/rebase/reset/pull forcé. Travail isolé
dans `/home/lolo/dev/msm/projects/MSM_vibe-R1-live-fram-configuration`, branche
`codex/r1-live-fram-configuration`, publication autorisée vers main. Les worktrees
historiques et deux stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et
`f3b829e6188f491920edf74a5f9ed0f559d08b87` sont préservés. **2576 fichiers protégés**
recomparés par SHA256, inchangés : gels, corpus R0, dettes, anciens travaux,
mission/gouvernance notamment. Archives R0 mission/rapport créées sans écrasement,
byte-identiques à `07c1769de0ded11dbac8bb6af576ce24788aaefc`.

## Changements et comportement observé

Commit de code évalué : **`f4697febfd5cea267a3c4fa21a1f1e0071845665`** —
`R1: add isolated LIVE configuration and transactional FRAM recovery`.
Les validations conservées portent sur les fichiers de ce commit ; seuls
rapport/journal/roadmap/archives/preuves sont ajoutés ensuite.

- Validateur LIVE séparé du V1 : tuple acquisition 2/stockage 0/limite 0/durée 0,
  domaines et labels R0, IDs, masque supervision 1, warning <= alarm. Aucune
  dépendance SD ni capacité de campagne fictive.
- Codecs configuration format 2 et identité format 2 explicites ; anciens
  décodeurs restent stricts. Générations, identité, CRC et miroirs B4 vérifiés.
- Média logique 3/taille 52298, superblocks 122880/126976 et banques
  131072/188416. Écritures exclusivement dans [122880,245760), callback de
  propriété obligatoire à chaque écriture ; absence/refus interdit l'écriture.
- Migration explicite hors boot, espace EMPTY seulement : copie contrôlée,
  identité protocole 2 préparée, TR2M generation/SHA256/UID, ancre TR2Q epoch 1 ;
  journal nouveau et BootIntent vides. Aucun effet historique rejoué ni
  configuration campagne convertie en active LIVE. Originaux conservés.
- Recovery choisit d'abord la racine publiée CRC-valide la plus récente puis sa
  banque ; pas de fallback après corruption de cette banque. Conflit de même
  génération/incompatibilité/erreur média refusés. Ancienne racine vers banque
  inactive réécrite ne bloque pas la publication courante. Changement de
  l'historique après rollback invalide la provenance et impose réconciliation.
- Application locale refusée avant I/O si acquisition en cours ou CRC/domaine
  incorrects ; plafond refusé sans écriture après recovery. Persistance avant publication. Erreur aux barrières :
  autorité retirée jusqu'à recovery, y compris commit durable dont la relecture
  échoue. Nouvelle corruption de slot refusée ; seul le marqueur historique
  endommagé byte-identique peut être ignoré.

**Frontière d'intégration R1 :** `LiveConfiguration` et projection B4 schema 2
sont des API de backend, sans raccordement au RTU de production. L'identité
préparée conserve seulement les bits historiques 2/3 : ni acquisition/SD/LIVE.
Le serveur cumulatif reste protocole 1. Aucune commande 12/13/14 ni B3 LIVE n'est
simulée. L'ancre epoch est une réservation versionnée ; R1 refuse journal/effets
ou epoch futurs plutôt que les rejouer. R2 doit intégrer le vrai moteur et les
versions B0/B4/B6 de manière cohérente avant d'exposer protocole 2. Ce choix
satisfait le découpage incrémental R1 sans annoncer un profil complet absent.

Documentation des API, exclusivité, partage du buffer et recovery :
[`05_Firmware/LIVE_R1.md`](../../05_Firmware/LIVE_R1.md).

## Validations exécutées et preuves

Preuves versionnées : [`preuves/TR2-20261015-R1`](preuves/TR2-20261015-R1/README.md),
logs bruts, empreintes du firmware évalué, contrôle des fichiers protégés,
ressources ARM et profils/empreintes de build.

| Validation réellement exécutée | Résultat / portée |
|---|---|
| Validation complète hôte, `cmake --build .../build-host-validation -j 8`, `ctest --test-dir .../build-host-validation --output-on-failure` | **129/129 PASS**, 11,70 s au dernier passage ; aucune suppression/atténuation de test |
| Test R1 avec AddressSanitizer + UndefinedBehaviorSanitizer | **PASS**, aucune erreur rapportée ; vrais codecs/média/commits, transport mémoire |
| CRC préparé/actif R0 V1_REFERENCE, LIVE_A, LIVE_B | **5207CCFC/092C7610**, **7BA855C8/7818AAB3**, **FC4001B4/E5F05014** ; tableaux exacts et miroirs |
| Domaines LIVE | **560 combinaisons PASS**, cas négatifs tuple, IDs, seuils, labels, masques, domaines ; V1 refuse LIVE |
| Coupures simulées | **407 budgets migration + 407 activation** ; payload tous les 256 octets, puis chaque octet du budget 52224 à 52426. Historique et tail inchangés à chaque cas. Activation : ancienne ou nouvelle active complète récupérable, aucune active partielle |
| Corruption/compatibilité | Banque publiée endommagée, format inconnu, conflits, record endommagé avec image saine, UID erroné, provenance périmée, futur epoch/effet/journal/capacités, propriété absente, compteurs U32 au plafond : refus vérifiés |
| Erreurs de lecture après chacune des 3 écritures du commit | **PASS** : aucune autorité conservée sur erreur ; reboot retrouve ancien état ou nouveau commit effectivement durable |
| SHA256 portable | Vide, abc, padding deux blocs et million de `a` conformes ; aucune certification cryptographique revendiquée |
| `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` | **PASS** Cortex-M33 cumulatif, nouveaux sources compilés ; fonctions backend non référencées absentes de l'ELF final |

Validation complète justifiée par modification du cœur, codecs et média partagés.
Le premier lancement direct `./tr2_validate.sh` a échoué (mode fichier non
exécutable), puis le lancement documenté avec `bash` a exécuté 129 tests PASS.
Son premier cross-build a détecté `-Werror=type-limits` : comparaison SHA256 à une
borne U64 impossible pour size_t 32 bits. Correction par garde de compilation
suivant SIZE_MAX, sans affaiblir les warnings. Rebuild/tests hôte complets puis
cross-build final réussis. Deux fixtures négatives ont été corrigées avant
validation finale : conflit initialement construit identique, mutation d'epoch
initialement identique ; les assertions de refus sont conservées. Aucun test
physique réussi antérieur n'a été répété.

Binaire cumulatif compilé **non flashé** : SHA256
`ac27b274bf526ba71c363fd1c331bb5e82048d8f19c192a601526560f759cbea` ;
ELF `29e096aa3cb50fe93e47da1295c8e4c6c81fc6546366e5be3310b1377c372101`.
Linker : text 170892, data 480, bss 602232 octets ; avertissements newlib de
syscalls non implémentés conservés dans le log, sans échec de linkage. Ces
artefacts prouvent une compilation, pas l'image présente sur la carte.

## Coût et limites mesurées

ARM GCC 14.2.1 Cortex-M33, -O0 : service **576**, média **96** (baseline **80**),
candidate **52298**, snapshot **132** octets. Design avec candidate partagée :
**53066 octets**, soit **+1072** par rapport à l'ancien média/candidate, hors
alignement et buffers des couches appelantes. La structure média partagée grossit
de 16 octets par instance, y compris historique ; aucun service/candidate LIVE
n'est instancié dans la production actuelle. Plus grand cadre statique observé
parmi les unités analysées : **440 octets** ; ce n'est pas une borne de pile
complète avec appels/HAL/IRQ ni une mesure high-water sur cible.

Commit : **52426 octets écrits** (52298 + 64 + 64), avec relectures et CRC.
Première application dans la fixture nominale : **313916 octets lus**, mesurés
par le transport simulé. Aucun délai physique dérivé de ces volumes n'est
revendiqué. Le backend synchrone refuse APPLY pendant acquisition ; R2/R4 doivent
qualifier ordonnanceur, réserves STOP/renouvellement, latences et marges FIFO.
Les coupures sont un modèle hôte borné, pas l'atomicité électrique ni l'endurance.
Une migration interrompue peut laisser un espace non EMPTY : arrêt et
réconciliation explicite, jamais formatage opportuniste.

## Matériel, journal et suite

**Aucune opération matérielle** : zéro flash/debug/reset, accès ou écriture FRAM,
commande SD, trafic RS-485. Autorisations commit/push exercées ; flash/debug non
exercées. La propriété/contenu de la zone LIVE réelle n'ont pas été déterminés,
aucun callback matériel fourni et aucune écriture tentée. Firmware, FRAM et SD
conservent leur état hérité **non relu**, sans prétendre restaurer M014-H à partir
d'une hypothèse. Le HP est retiré selon gouvernance §10 ; tout essai futur doit
inventorier le banc unique T1000 et son maître effectif.

Journal `ETAT_COURANT_TR2.md` et roadmap actualisés : R0 reçu selon contrat R1,
configuration/persistance validées logiciel, frontières R2/R3, coûts R4 et absence
de qualification matérielle explicites. Dettes A-04 et corpus gelés préservés.

Suite recommandée à ChatGPT : contrôle indépendant du commit, puis mission R2
moteur epoch/journal/START_LIVE/STOP_LIVE et intégration de protocole cohérente,
sans se contenter de l'ancre R1. R3 calcul, R4 marges et R6 physique restent à
qualifier. Avant provisioning : établir propriétaire et contenu réels de toute
la zone LIVE, sauvegardes appropriées, attribution UID/DeviceId et cible.

## Publication

État avant publication : commit de code local créé, rapport/preuves/journal en
préparation sur le worktree isolé ; dépôt principal et stashes préservés.
Destination vérifiée `https://github.com/loloLR17/MSM_vibe.git`, push normal
`HEAD:refs/heads/main`. Relecture distante encore à obtenir ; statut
IN_PROGRESS limité à cette attente, pas un blocage externe. Le SHA contenant le
rapport sera communiqué après publication, jamais inventé dans son propre commit.
