---
mission_id: TR2-20261013-M015-P-COMPARATIVE-FIFO-ROOT-CAUSE
status: BLOCKED
base_ref: main
base_sha: c3e85afbd32f38f07687d2f12cf07a96fc2ed6ee
initial_head: c14d3c60f4c42dc7eba2a1712633c656b18965a8
result_sha: null
created_at_utc: 2026-10-12T22:21:25.445600+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-P — checkpoint comparaison et test causal préparé

[Comparatif détaillé I/J/O](../../../experiments/iis3dwb_st_reference/COMPARAISON_M015_P.md)
avec chemins/commits/lignes,transactions etmatrice. Iraw1794→256XYZréels,
CSV byte-identique,BIN/ELFhistoriques exacts ;O258TS,aucunXYZ. >>2 réfuté parles
histogrammesbruts. J128mots tagXYZreçus maisrejetés après09=14/0A=0C ;ce n'estpas
uneabsence de réceptionXYZ. Cartes/modules etplusieursfacteursdiffèrent.

Unehypothèse retenue :framingSPIHAL commande/payloadséparévsunappelduplexI/J.
DeuximagesidentiqueshorsBENCH_DUPLEX0/1 sontconstruites,capconfigsST inchangées,
mode3/CPU16/SPI2/level10/O2HAL identiques. Source/SDK/vendorpréservés.
Unnouveautestciblé passe,couvrantanciens14+8budgetset8assertionsdeframes ;
2cross-buildsARM réussis. Aucuntestanciennechaîne/production inchangée rejoué.
Faits/hypothèsedistincts,aucunmatérielP encore ;uncontrôle etuncausal maximum,
STOP anomalienouvelle,pasportagemultifactorielI ouflashprimaire.

Archivesmission/rapportO exactes,worktrees/stashes préservés. Checkpointlogiciel
à publier etrelire avantcible ;BLOCKED provisoire pourpreuvesphysiques manquantes,
pasattente utilisateur. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z` et
[preuves versionnées](preuves/TR2-20261013-M015-P/).
