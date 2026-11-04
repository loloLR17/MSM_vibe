# Clôture administrative V1.5 — transition vers étude ciblée de visibilité TR2

Date : 2026-11-04. Référence : `loloLR17/MSM_vibe`, `main`.

Mission précédente `TR2-20261104-SHIELD-V15-INTEGRATION-3D` : rapport Codex publié `BLOCKED`, code/mechanics évalués au commit `95e95b51445ae387c4a2a3cb4ca4d0719c8d0e4a`, rapport au commit `7cea02d42fc505a9b05c4ab5acca458bbd4ebbe3`. Revue indépendante ChatGPT au commit `8f0f91faf68b761905eb4fb77b89b4ae7ed7000c`. **Le statut historique BLOCKED et les réserves ne sont pas transformés en DONE.**

Acquis : maquette éditable `mechanical/V1_5/`, boîtier conceptuel, STEP/coupes/vues, contrôles géométriques nominaux, plan de contrôle physique. Le PCB électrique V1.4 est resté inchangé ; fabrication **NE PAS COMMANDER**. La commande ne peut être libérée sans vérifier l'ajustement C05/Zio/J5 et les interfaces susceptibles de modifier le shield.

**Décision de poursuite utilisateur** : l'état du TR2 doit être lisible **sans ouvrir le boîtier** ; conserver les quatre états déjà signalés par les **trois LED Nucleo** (LD1 verte/PC7, LD2 bleue/PB7, LD3 rouge/PG2), et obtenir une **étude ciblée, proportionnée** du moyen le plus simple d'y parvenir. Préférence pour des guides de lumière **seulement s'ils restent simples** (courts/droits, découpe locale compatible), sinon envisager des LED reportées sur shield ou une autre solution sobre. Étude avant adoption de changements électriques ou mécaniques structurants, sans refonte firmware ni achat. L'utilisateur a fourni des photos rapprochées des LED et de USER avec une règle ; les photos sont dans le chat et ne sont pas présumées disponibles dans l'environnement local de Codex, qui doit indiquer si les mesures quantitatives nécessaires ne sont pas vérifiables.

Cette clôture est **administrative** afin de remplacer le contrat actif V1.5 par un contrat distinct d'étude LED. Elle n'autorise ni fabrication, ni manipulations STM32, ni commande ni impression. Les autres éléments d'intégration (USER, microSD, RS485, CN15) restent réservés et ne doivent pas être dégradés.
