---
mission_id: TR2-20261016-R4M-LIVE-FRAM-RECONCILIATION
status: IN_PROGRESS
base_ref: main
base_sha: 0e09805f4ade74ed04553f04efb7a881e8bfb84b
initial_head: 11719e3946e0aa0773e8993b841c82760130fcf3
result_sha: 3606942239f54930249d024dc33d1812d8629879
created_at_utc: 2026-10-16T08:39:36Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-M — checkpoint avant maintenance physique

PhaseA acquise : identité ST-LINK/UID conforme, doubles sauvegardes fraîches Flash2Mio
et FRAM262144 exactes, égales à l'état final R4. Flash SHA c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26,
FRAM SHA c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561.
WHO7B,CTRL1=00,FIFO_CTRL4=80/lowbits0,CTRL10=00 relus. Aucun flash/reset/writeNVM.
GDB limité à sauvegardes/lectures/RAM puis détaché. Backups local reports/TR2-r4m/hardware.
Archive R4 mission+rapport byte-identiques à0e09805 avant remplacement ; stashes/worktrees/gels conservés.

[Procédure détaillée](../../05_Firmware/LIVE_R4M.md) et [preuves](preuves/TR2-20261016-R4M/README.md).
Ancien STARTED **reste indécidable**, aucune terminalisation fictive. Copie journalA
RESERVED/gen1 etB STARTED/gen2, clé1/1/code13/run1/config2 ; deux INTENT authentiques.
Archive intégrale FRAM sans perte1273octets zlib dans tests/fixtures, SHA vérifié.

Méthode : nouveau contexte isolé, journal staged/commit dans LIVE seulement,
INTENT128octets etepoch1/configs inchangés ; frontend CLOSED/RETIRING à chaqueboot,
seul vrai RENEW1->2 autorisé, ouverture après retraite durable. Prochainrun2,
ancienneclé refus24. Aucun bridge exposé à un serveur produit ; contrôle négatif
prouve que cette restriction de firmware est indispensable. Reset sur bridge :
WAIT fermé et SHA source refusé, aucune reprise/initialisation en boucle.

Nouveau test de maintenance PASS1,97s :142coupures du commit+4phasesRENEW, origine
réelle et vrais stores/moteur, absence de faux reçu, anti-rejeu/floor et refusdoubles.
ASan/UBSan ciblé PASS1,16s. **Pas137tests déclarés exécutés** : les136anciens restent
acquis sur code cœur inchangé. Cross-build canonique etprobeARM PASS ; BINcanonique
inchangé6a2150af8939ead0b81219a96cfd51dc2cbef03a3851d46775fa71ddbfeb15a9.
Rien de cela ne constitue encore une validation physique de maintenance ouR4.

Prochaine tranche autorisée : flash standard du probeR4M issu du code ci-dessus,
WAIT/UID/readback ; tokenbackup puis unique trigger4 pour maintenance+campagneR4
bornée. Journal d'adresses, latences/STOP/FIFO et faute de service injectée après
COMPLETED (pas faute électrique SPI). Après succès seulement, reset et trigger5
readonly pour recovery/absence d'autostart. Arrêt si nouveau STARTEDindéterminé,
aucune réinitialisation enboucle. Fin : sauvegardeFRAM nouvelle, restaurationFlash
initiale exacte2Mio ; **aucun rembobinageFRAM** des nouveauxepochs/run_ids.
AucunSD/HP/COM7/RTUélectrique ni provider temps capteur arbitraire. B3/LIVE2/R6nonqualifiés.

Commit logiciel bc3e83de050a527cc839b494025c906f5c06246b, branchecodex/r4m-live-fram-reconciliation ; publication et
relecture du checkpoint puis poursuite physique. Le contrat courant reste inchangé.


## Checkpoint physique intermédiaire — reprise fermée du pont

Le trigger4 réel a publié une unique isolation LIVE (52426 octets,1639 commandes,
3 plages LIVE) puis s'est arrêté avant RENEW/START : work1075us>1000us, erreur6,
aucun échantillon ni nouvelle commande. Capteur arrêté ; snapshot262144 préservé,
SHA c868950fe3c60f9a3b23a9ce8d565029ee57d4f06b08cbf6062de9238b2af950,
historique/tail inchangés. Observation GDB a fini en erreur de présentation pour
un tableau de0opérations ; les résultats précédents et le snapshot sont acquis.
Relecture seule en RAM réussie824us/0write ; cause exacte du dépassement nonattribuée.

Correction logicielle : checkpoints entre validations/décodages successifs,
contrôles/limites inchangés.137tests complets PASS56,30s, sanitizer maintenance
PASS1,38s, cross-build canonique/probe PASS. Réexécution137 justifiée par correction
core ; aucun ancien test physique rejoué.

Plan de poursuite documenté dans LIVE_R4M : trigger6 explicite accepte uniquement
le SHA du pont inspecté et l'INTENT128authentique. Frontend reste CLOSED jusqu'à
ce contrôle, puis RETIRING/unique vraiRENEW1->2. **Aucun second reprovisionnement**.
Nouveau snapshot vérifié avant token ; arrêt impératif si nouvelle indétermination.
Cette tranche est préparée, pas encore exécutée. Firmware initial sera restauré.
Code de reprise : 3606942239f54930249d024dc33d1812d8629879
