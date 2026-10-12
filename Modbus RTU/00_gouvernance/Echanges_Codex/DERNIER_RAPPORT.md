---
mission_id: TR2-20261012-M014-H-PRODUCTION-RESUME
status: BLOCKED
base_ref: main
base_sha: 30f4663c7eece9ffb66740da1466c0ae88432f5a
initial_head: b3a4ac7125b59f57979abd0a1458c11e52515504
result_sha: null
created_at_utc: 2026-10-12T07:46:16.958236+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-H — Point avant provisioning physique

M014 IN PROGRESS. Rapport incrémental de conservation ; BLOCKED couvre les
critères physiques et la relecture distante encore à obtenir, pas une demande
d'approbation utilisateur. Contrat actif inchangé, archives G exactes depuis
30f4663 ; deux stashes conservés. Main initial b3a4ac7, arbre propre.

Acquis G réutilisés : 113 tests firmware + cross, 539 .NET non relancés sur code
inchangé. UID/ST-LINK/probe courant vérifiés via GDB seul ; Flash2Mio et RAM768K
préservées, Flash courante comparée identique au dump G. Première commande dump
GDB UNC échouée ; corrigée avec chemin forward-slash qualifié, aucun reset/write.

Livrés : manifeste H opérationnel pour supervision dégradée, générateur registre
vers identité/header et B0 attendu, strong binding durable STM32, profil
manufacturing explicitement déclenché et profil production hors harness. Aucune
instruction HAL SD read/write liée dans les images. Aucune allocation SD et
aucune modification de sa partition. Identité versions attribuées hw1/fw0.14.8/
proto1/caps0xC ; origine projet documentée, pas mesure ni fixture.

La configuration persistée est validée **structurellement seulement**, sans
prétendre à une capacité usable : boot normal environment=false/capacité0,
configuration UNAVAILABLE, disponibilité stockagefalse/READYfalse. Les fonctions
campagnes indisponibles refusent et ne remplacent pas le backend bulk par FRAM.
Cette réserve est explicite dans manifeste et procédure ; pas configuration
opérationnelle ou acquisition qualifiée.

Résultats nouveaux : deux cross-builds dédiés réussis ; six tests du générateur
réussis ; répétition offline sur copie du backup FRAM réel réussie : refus sans
autorisation/mauvaisUID, générations10→11→12, six writes, autres domaines
inchangés, B0 exact, configuration UNAVAILABLE sans ressource. Compilation du
check hôte initialement échouée pour include ConfigurationStore manquant,
corrigée ; pas assertions affaiblies. Cross-build du profil par défaut requis
par modifications main/deps STM32 ; retour0 obtenu (default-cross-validation.log). La commande tr2_validate.sh se lance depuis la racine Git ; la mention de cwd firmware dans le rapport G était inexacte.

**Aucune écriture FRAM physique encore faite, aucun flash H, aucun COM7/RS485.**
Restent flash manufacturing qualifié, backup FRAM cible byte-equal et trigger
unique, readback complet/boot production, paquetHP OFFLINE puis essais bornés
B0/B1 polling/perte/reprise. Procédure dans COMMISSIONING_PRODUCTION_M014_H.md.
Preuves : /home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z. Résultats G et E conservés,
M013/C/I non rejoués ; limites timing/DE/TC/robustesse/E4 inchangées. Ne pas
annoncer DONE sur compilation ou provisioning seul.

## Intégration concurrente contrôlée

Push82169af refusé après mise à jour ChatGPT43f37db (arbitrage SD/communication). Autorisation utilisateur explicite de réappliquer la tranche dans un worktree isolé puis commit/push normal. Toutes les sources réappliquées sont byte-identical au commit évalué ; aucune validation répétée pour ce déplacement. Dépôt initial et commit82169af préservés ; worktree de publication MSM_vibe-M014-H sur branche locale codex/m014-h-integration, destination explicite main. Contrat clarifié43f37db respecté.
