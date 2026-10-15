# R4 — preuves logicielles et qualification physique progressive

Voir DERNIER_RAPPORT.md et LIVE_R4.md pour conclusions/limites. Logs bruts conservés,
y compris erreurs d’outillage et essais refusés ; un code de retour global ne remplace
pas chaque résultat. final-full-validation-bash.log :136tests+ARM PASS ;
final-sanitize-test.log :3suites PASS. burst-test.log : modèle UART renforcé.

hardware : inventaire, doubles sauvegardes vérifiées, commandes/logs flash et GDB,
essais1(timeout1ms),2(premiermot>5ms),3(préfixe continu et timeout du contrôle
final interrompu par GDB). bench3-stopped-full-fram-control.log prouve la relecture
complète à l’arrêt ; comparaison Python binaire exacte conservée dans le script.
R1/R2 sont ensuite déclenchés sans halt pendant capture. Aucun trafic RS485/SD.
Ne pas confondre préfixe valide et RUN entier PASS.

Les binaires Flash/FRAM restent locaux dans
/home/lolo/dev/msm/reports/TR2-r4/hardware ; leurs empreintes sont publiées,
mais ne remplacent pas le contenu. Scripts .py/.gdb permettent la reproduction
sur la cible identifiée, avec permissions matérielles et préflight requis.
SHA256SUMS.json couvre intégralement les preuves publiées sauf lui-même.
Ressources .su : cadres statiques O0 ARM, pas high-water physique de pile.
