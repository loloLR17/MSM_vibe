---
mission_id: TR2-20261118-SHIELD-V24-SPI3-GERBER-CORRECTIF
status: IN_PROGRESS
base_ref: main
base_sha: dcd3be60e147d610e171ff2ca37f099e35054dfe
initial_head: fb7167548dc1f55eb8250d3d52c8c85dd5caa8c9
result_sha: null
created_at_utc: 2026-11-18T21:08:46.114035+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Premier checkpoint — achats urgents préparés, correction CAO en cours

[Liste d'achat et risques de délai](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/ACHATS_URGENTS.md), [CSV quantités/MPN](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/PREPARATION_2_SHIELDS.csv), [sources datées](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/SOURCES_COMMERCIALES.json).

Livrable intermédiaire demandé : besoin pour deux shields sans présumer les connecteurs soudés récupérables, modules réutilisés sans rachat, stock non soudé inconnu et question utilisateur en cours. Disponibilités/prix publics TME consultés ; limitesDigiKey/Mouser explicites. Les options non encore validées M-01 ne sont pas adoptées. Délai expressTME1–2jours ouvrables/19,08€TTC avant14hCET : réception avant samedi21 **non garantie** ; scénario lentlundi23. Aucun achat, paiement, authentification, téléchargement fabricant ou modification matérielle.

Worktree distinct `MSM_vibe-SHIELD-V24-SPI3-CORRECTIF`, initial fb716754. Sources V2.3 copiées pour correction dans variante séparée ; les fichiers CAO copiés, encore non corrigés, ne font pas partie de ce checkpoint. Référence primaire ST officielle SPI3 consultée par outil Web ; téléchargement directPDF a expiré, ce qui n'est pas un fichier téléchargé/haché. Suite : correction schéma/symbole/PCB/routage, E-01 exhaustif180contacts, M-01 géométrie exacte incluant vias, puis CAMcandidat unique et relecture distante des octets.

AGENTS/protocole/journal courant identiques byteexact à la référence intégralement consultée de la mission précédente, vérifiés par empreintes ; nouvel invariantM-01 lu. Revue indépendante RS485 f4131e80 lue, acquis préservés sans rejeu. Journal actualisé avec cette reprise/limites ; racinehistorique82169af7, worktrees et deux stashes préservés. Aucun flash/debug/COM/média. Diff documentaire et liens contrôlés ; pas de buildfirmware justifiable. Contrat humain et archives existantes inchangés. Publication normale et relecture de ce checkpoint à vérifier avant poursuite de la CAO ; il ne constitue ni un résultat électrique correctif ni une libérationCAM.
