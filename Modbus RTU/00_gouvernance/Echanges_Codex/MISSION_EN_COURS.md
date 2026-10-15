---
mission_id: TR2-20261015-R0-NORME-LIVE-SUCCESSEUR
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R0 — Finalisation normative du profil LIVE sans stockage

## Contexte
Décision utilisateur du 15/10/2026 : la supervision vibratoire doit fonctionner de façon continue sans carte SD ; la sauvegarde des campagnes demeure prévue, mais ne bloque plus la supervision. Le contrat R0 et la roadmap R0–R7 sont publiés sur main. La mission d'audit A-06 est terminée et son rapport DONE est publié ; archiver ce contrat et ce rapport sans perte avant de remplacer les livrables courants.

## Mission
Finaliser le contrat normatif successeur, le mapping versionné et les oracles de validation, **sans implémentation firmware ni essais matériels**. Lire la gouvernance courante AGENTS.md et GOUVERNANCE_CHATGPT_CODEX.md, le contrat R0, l'étude A-06, l'état, la roadmap, les blocs 0–7, V1.1 et les FT/TT actifs. Vérifier HEAD réel de main et état Git avant intervention.

1. Définir des valeurs numériques non conflictuelles de version protocole et structure de configuration, bit de capacité B0, tuple B4 LIVE, codes B5 distincts des codes campagne 3/4 et du code 12 déjà alloué en V1.1 ; B3 bit8 et santé B6. Préserver types, adresses, longueurs, endian et CRC V1.
2. Définir la supervision LIVE **sans durée limite imposée par les campagnes**, son encodage B4, l'arrêt explicite, les comportements de reboot, rollover et recovery ; jamais de redémarrage automatique implicite.
3. Décrire états et transitions B1/B3/B6/B7, fautes et alarmes, absent volontaire versus panne SD, refus croisés, idempotence, fraîcheur et validité ; préserver les diagnostics réels.
4. Spécifier la migration FRAM et le rollback sûrs à partir du layout effectivement présent ; vérifier compatibilité ancien/nouveau firmware et ancien/nouveau PC, coupures d'alimentation, CRC préparé et actif et vecteurs de référence.
5. Produire une spécification d'extension distincte des gels V1, un mapping successeur et une matrice de tests successeur couvrant FT-LIM/CMD/INT/BLK/SEQ/PER/OBS et PC. Identifier les ambiguïtés restantes et les décisions structurantes seulement.
6. Actualiser la roadmap R0 factuellement, sans déclarer un jalon qualifié sans preuve.

## Critères d'acceptation
Cohérence des versions et réservations démontrée ; tables d'états/refus exhaustives pour le périmètre ; invariants V1/V1.1 et vecteur CRC historique 0x5207CCFC préservés ; nouveaux vecteurs LIVE reproductibles ; migration/rollback documentés ; traçabilité exigences vers tests ; revue de cohérence et liste des risques ouverts. Les spécifications, mappings et fiches gelées historiques ne doivent pas être modifiés. Aucun code applicatif, flash, debug, accès matériel, ni qualification physique. Ne pas déclarer R0 clôturé si une contradiction normative subsiste.

## Livrables et publication
Publier les documents successeurs et preuves documentaires sur main, mettre à jour DERNIER_RAPPORT.md, archiver A-06, commit/push puis vérifier les fichiers et SHA distants. Rapport clair sur travaux réalisés, preuves, arbitrages et conditions d'entrée en R1. Circuit ChatGPT → GitHub → Codex → GitHub → ChatGPT ; respect d'A-06 et des autorisations permanentes de AGENTS.md.
